#!/bin/python3

#> Imports
import click
import parglare
from enum import IntEnum
from pathlib import Path
from dataclasses import dataclass

from . import builtin_grammar
#</Imports

#> Header >/
class ExitCode(IntEnum):
    SUCCESS = 0
    GENERIC_FAIL = 1
    # 2 is sometimes reserved for misuse of shell builtins,
    # according to BASH documentation
    BUILTIN_GRAMMAR_MISSING = 2

    def raise_with_msg(self, msg: str, *, print_ec: bool = True, color: str = 'red', **kwargs) -> None:
        click.secho(f'EC {self!r}: {msg}' if print_ec else msg, color=color, **kwargs)
        raise click.exceptions.Exit(self.value)

@dataclass(slots=True, kw_only=True)
class SharedState:
    grammar_path: Path | None

    def load_grammar(self) -> parglare.Grammar:
        if self.grammar_path is None:
            gp = builtin_grammar('canonical/canonical.pg')
            if not gp.is_file():
                ExitCode.BUILTIN_GRAMMAR_MISSING.raise_with_message(f'No grammar was supplied, but the builtin grammar was not found'
                                                                    f'\n At: {gp}')
        else: gp = self.grammar_path
        return parglare.Grammar.from_file(gp)

@click.group()
@click.option('-g', '--grammar', type=click.Path(exists=True, dir_okay=False, path_type=Path), default=None,
              help='The grammar file to load (defaults to the built-in canonical grammar, if present)')
@click.pass_context
def cap(ctx: click.Context, *, grammar: Path | None) -> None:
    ctx.obj = SharedState(grammar_path=grammar)


if __name__ == '__main__': cap()
