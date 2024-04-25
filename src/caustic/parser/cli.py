#!/bin/python3

'''Provides a CLI for parsing'''

#> Imports
import sys
import click
import typing
from pathlib import Path
from parglare import Parser, GLRParser
from parglare.exceptions import ParseError

from . import CausticParser, SourceInfo
from .error import format_exc
#</Imports

#> Header
def default_grammar() -> Path:
    try: from caustic import grammar
    except ImportError as e:
        raise RuntimeError('Cannot load default grammar') from e
    base = Path(grammar.__file__)
    if base.is_file(): base = base.parent
    if not base.is_dir():
        raise NotADirectoryError(f'Cannot load default grammar: {base} is not a directory')
    file = base/'default.pg'
    if file.is_file(): return file
    raise FileNotFoundError(f'Cannot load default grammar: {file} is not a file')

def cst_to_pickle(top: 'CausticASTNode') -> bytes:
    from caustic.cst import serialize
    return serialize.serialize(top)

def _cst_to__json(top: 'CausticASTNode', **kwargs) -> bytes:
    import json
    from dataclasses import asdict
    return json.dumps(asdict(top), **kwargs).encode()
def cst_to_short_json(top: 'CausticASTNode') -> bytes:
    return _cst_to__json(top)
def cst_to_pretty_json(top: 'CausticASTNode') -> bytes:
    return _cst_to__json(top, indent=4) + b'\n'

def cst_to_pretty(top: 'CausticASTNode') -> bytes:
    import pprint
    class CSTPrettyPrinter(pprint.PrettyPrinter):
        # copied from pprint.py
        def _format(self, object, stream, *args, **kwargs):
            if isinstance(object, SourceInfo):
                stream.write(repr(object))
                return
            super()._format(object, stream, *args, **kwargs)
    return CSTPrettyPrinter().pformat(top).encode() + b'\n'

@click.command('Caustic Parser')
@click.argument('source', type=click.Path('r', dir_okay=False, allow_dash=True, path_type=Path), default='-')
@click.option('-g', '--grammar', type=click.Path(exists=True, dir_okay=False, path_type=Path), help='Alternative grammar file', default=None)
@click.option('--glr', help='Use a GLR parser instead', is_flag=True, default=False)
@click.option('-o', '--output', type=click.File('wb'), help='Where to write the output (defaults to a filename depending on the SOURCE and format, use "-" for STDOUT)', default=None)
@click.option('-f', '--format', type=click.Choice(('pickle', 'json', 'json-pretty', 'pretty', 'tree', 'multitree')), help='What format to write as', default='pickle')
@click.option('-q', '--quiet', help='Don\'t output status messages unless a failure occurs', is_flag=True, default=False)
@click.option('--debug', help='Put the parser in "debug" mode (see ParGlare docs)', is_flag=True, default=False)
def cli(*, source: Path, grammar: Path | None, output: typing.BinaryIO | None, format: typing.Literal['pickle', 'json', 'json-pretty', 'pretty', 'tree', 'multitree'],
        quiet: bool, glr: bool, debug: bool) -> None:
    '''
        Parses a Caustic source file into a CST for compiling

        SOURCE is the file to compile (defaults to STDIN)

        Note that the "pickle" format is the only format that can be used by the (default) compiler
            "pretty" is recommended for viewing (without post-processing)
        Note that the "tree" format will force the LR (the default parser if --glr is not set) parser to generate a tree
    '''
    error = lambda *a,**kw: click.echo(click.style(*a, (255, 63, 63)), color=True, file=sys.stderr, **kw)
    debug_ = (lambda *a,**kw: None) if quiet else (lambda *a,**kw: click.echo(*a, file=sys.stderr, **kw))
    real_source = str(source) != '-'
    tree_fmt = format.startswith('tree')
    # Load grammar
    if grammar is None:
        grammar = default_grammar()
        debug_(f'Default grammar discovered at {grammar}')
    debug_(f'Loading grammar and constructing parser from {grammar}')
    parser = CausticParser.from_file(grammar, parser_type=(GLRParser if glr else Parser), build_tree=(tree_fmt or glr), debug=debug, lexical_disambiguation=(format != 'multitree'))
    # Parse data
    try:
        parsed = (parser.parser.parse_file(source) if real_source
                  else parser.parse(sys.stdin.read(), file_name=None))
    except ParseError as e:
        error(format_exc(e))
        sys.exit(1)
    # Handle GLR
    if glr:
        if format != 'multitree':
            if parsed.ambiguities:
                error(f'Post- GLR parse failure: ambiguous parse ({parsed.ambiguities} ambiguit(y/ies))')
                sys.exit(1)
            if parsed.solutions > 1:
                error(f'Post- GLR parse failure: multiple solutions ({parsed.solutions} solutions)')
                sys.exit(1)
            parsed = parsed.get_first_tree()
            if not tree_fmt:
                parsed = parser.parser.call_actions(parsed)
    # Encode data
    match format:
        case 'pickle':
            data = cst_to_pickle(parsed)
            suff = 'pkl'
        case 'json':
            data = cst_to_short_json(parsed)
            suff = 'json'
        case 'json-pretty':
            data = cst_to_pretty_json(parsed)
            suff = 'json'
        case 'pretty':
            data = cst_to_pretty(parsed)
            suff = 'pretty.txt'
        case 'tree' | 'multitree':
            data = parsed.to_str().encode() + b'\n'
            suff = 'tree.txt'
        case _: raise ValueError(f'Unknown format: {format!r}')
    # Output
    if output is None:
        output = click.open_file((source.with_suffix(f'.cst.{suff}') if real_source else '-'), 'wb')
    debug_(f'Wrote {output.write(data)} byte(s) to {output.name}')
#</Header

#> Main >/
if __name__ == '__main__': cli()
