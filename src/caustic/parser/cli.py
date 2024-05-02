#!/bin/python3

#> Imports
import sys
import json
import click
import typing
import parglare
import importlib
from enum import IntEnum
from pathlib import Path
from dataclasses import dataclass

from . import builtin_grammar
from .error import format_exc
#</Imports

#> Header >/
class ExitCode(IntEnum):
    SUCCESS = 0
    GENERIC_FAIL = 1
    # 2 is sometimes reserved for misuse of shell builtins,
    # according to BASH documentation
    BUILTIN_GRAMMAR_MISSING = 3
    CST_MISSING = 4
    ACTIONS_MISSING = 5
    AMBIGUOUS_GLR = 6
    GLR_MULTIPLE_SOLUTIONS = 7
    PARSE_FAILED = 8

    def raise_with_msg(self, msg: str, *, print_ec: bool = True, color: str = 'red', **kwargs) -> None:
        click.secho(f'EC {self.value} ({self.name}): {msg}' if print_ec else msg, color=color, file=sys.stderr, **kwargs)
        raise click.exceptions.Exit(self.value)

@dataclass(slots=True, kw_only=True)
class SharedState:
    verbosity: typing.Literal[-1, 0, 1]
    grammar_path: Path | None

    def load_grammar(self) -> parglare.Grammar:
        if self.grammar_path is None:
            gp = builtin_grammar('canonical/canonical.pg')
            if not gp.is_file():
                ExitCode.BUILTIN_GRAMMAR_MISSING.raise_with_message(f'No grammar was supplied, but the builtin grammar was not found'
                                                                    f'\n At: {gp}')
        else: gp = self.grammar_path
        if self.verbosity >= 0:
            click.echo(f'Loading grammar from {gp} ...', file=sys.stderr)
        return parglare.Grammar.from_file(gp)

@click.group(context_settings={'show_default': True})
@click.option('-g', '--grammar', type=click.Path(exists=True, dir_okay=False, path_type=Path), default=None,
              help='Grammar file to load (defaults to the built-in canonical grammar, if present)', show_default=False)
@click.option('-v/-q', '--verbose/--quiet', 'verbosity', default=None, help='Output more or less', show_default=False)
@click.pass_context
def cap(ctx: click.Context, *, grammar: Path | None, verbosity: bool | None) -> None:
    '''CAP -- CAustic Parser'''
    ctx.obj = SharedState(grammar_path=grammar, verbosity=(1 if verbosity else (0 if verbosity is None else -1)))

@cap.command
@click.pass_obj
@click.option('-j', '--json', 'as_json', is_flag=True, default=False,
              help='Output as JSON')
def imports(state: SharedState, *, as_json: bool) -> None:
    '''List all files and names imported by a grammar'''
    gram = state.load_grammar()
    if as_json:
        click.echo(json.dumps(
            {f: {n: i.file_path for n,i in sg.imports.items()}
             for f,sg in gram.imported_files.items()}, indent=4))
        return
    for f,sg in gram.imported_files.items():
        click.echo(f'{f}:', nl=False)
        if sg.imports:
            click.echo()
            for n,i in sg.imports.items():
                click.echo(f'- {n} @ {i.file_path}')
        else: click.echo(' <no imports>')

@cap.command
@click.pass_obj
@click.option('--glr/--no-glr', default=True, help='Use the GLR parser')
@click.option('--cst', metavar='FQN', default='caustic.cst', help='CST module to use')
@click.option('--actions', metavar='FQN', default='caustic.parser.actions', help='Actions module to use')
@click.option('-o', '--output', type=click.File('wb'), default='-', help='File for output [default: <STDOUT>]', show_default=False)
@click.option('-f', '--format', type=click.Choice(('pickle', 'pretty', 'tree')), default='pickle', help='Output format')
@click.argument('infile', type=click.Path(exists=True, dir_okay=False, path_type=Path), default=None, required=False)
def parse(state: SharedState, *, glr: bool, cst: str, actions: str, output: typing.BinaryIO, format: typing.Literal['pickle', 'pretty', 'tree'], infile: Path | None):
    '''
        Parse Caustic source into Caustic nodes using a grammar

        INFILE is the file to read from, defaulting to <stdin> when not given
    '''
    tree_format = format == 'tree'
    if state.verbosity >= 1:
        click.echo(f'Importing CST from {cst}', file=sys.stderr)
    try: cst = importlib.import_module(cst)
    except ImportError:
        ExitCode.CST_MISSING.raise_with_msg(f'Could not import CST at {cst!r}')
    if state.verbosity >= 1:
        click.echo(f'Importing actions from {actions}', file=sys.stderr)
    try: actions = importlib.import_module(actions).action
    except ImportError:
        ExitCode.ACTIONS_MISSING.raise_with_msg(f'Could not import actions at {actions!r}')
    except AttributeError:
        ExitCode.ACTIONS_MISSING.raise_with_msg(f'{actions!r} was imported, but does not have the "action" name')
    grammar = state.load_grammar()
    if state.verbosity >= 0:
        click.echo(f'Loading {"GLR" if glr else "LR"} parser', file=sys.stderr)
    parser = (parglare.GLRParser if glr else parglare.Parser)(grammar, actions=actions.all, lexical_disambiguation=not tree_format)
    if state.verbosity >= 0:
        click.echo(('Parser ready, waiting for input from <stdin>'
                    if infile is None else f'Parser ready, parsing file {infile}'), file=sys.stderr)
    try: parsed = (parser.parse(sys.stdin.read(), file_name='<stdin>')
                   if infile is None else parser.parse_file(infile))
    except parglare.exceptions.ParseError as pe:
        ExitCode.PARSE_FAILED.raise_with_msg(format_exc(pe))
    if state.verbosity >= 1:
        click.echo('Parse complete', file=sys.stderr)
    if glr and not tree_format:
        if parsed.ambiguities:
            ExitCode.AMBIGUOUS_GLR.raise_with_msg(f'Post- GLR parse failed -- ambiguous parse ({parsed.ambiguities} ambiguit(y/ies))')
        if parsed.solutions > 1:
            ExitCode.GLR_MULTIPLE_SOLUTIONS.raise_with_msg(f'Post- GLR parse failed -- multiple solutions ({parsed.solutions} solutions)')
        parsed = parsed.get_first_tree()
        if not tree_format:
            parsed = parser.call_actions(parsed)
    match format:
        case 'pickle':
            data = cst.serialize.serialize(parsed)
        case 'pretty':
            data = f'{__import__("pprint").pformat(parsed)}\n'.encode()
        case 'tree':
            data = f'{parsed.to_str()}\n'.encode()
    count = output.write(data)
    if state.verbosity >= 0:
        click.echo(f'Wrote {count} byte(s) to {output.name}', file=sys.stderr)

if __name__ == '__main__': cap()
