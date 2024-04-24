#!/bin/python3

'''Provides the `CausticParser` class'''

#> Imports
import typing
import parglare
from pathlib import Path
from collections import abc as cabc
#</Imports

__all__ = ('CausticParser', 'SourceInfo', 'error', 'cli')

#> Package
from . import error
#</Package

#> Header >/
class SourceInfo(typing.NamedTuple):
    '''Represents the source that a CST node came from'''

    filename: str | None
    start_pos: int
    start_line: int
    start_col: int
    end_pos: int
    end_line: int
    end_col: int

    @classmethod
    def from_ctx(cls, ctx: object) -> typing.Self:
        '''Generates a `SourceInfo` from a ParGlare context'''
        sl,sc = parglare.pos_to_line_col(ctx.input_str, ctx.start_position)
        el,ec = parglare.pos_to_line_col(ctx.input_str, ctx.end_position)
        return cls(filename=ctx.file_name,
                   start_pos=ctx.start_position, start_line=sl, start_col=sc,
                   end_pos=ctx.end_position, end_line=el, end_col=ec)

    def __repr__(self) -> str:
        return f'{type(self).__name__}({self!s})'
    def __str__(self) -> str:
        return (f'{self.filename or "<unknown>"}:'
                f'{self.start_line},{self.start_col}'
                f'-{self.end_line},{self.end_col}')

class CausticParser:
    '''Parses Caustic source code into a CST'''
    __slots__ = ('parser',)

    parser: parglare.Parser

    def __init__(self, parser: parglare.GLRParser):
        self.parser = parser

    @classmethod
    def from_grammar(cls, grammar: parglare.Grammar, *,
                     parser_type: type[parglare.Parser] = parglare.Parser, **parser_kwargs) -> typing.Self:
        '''Creates a `CausticParser` from a grammar'''
        return cls(parser_type(grammar, **parser_kwargs))
    @classmethod
    def from_file(cls, f: Path, **kwargs):
        '''Creates a `CausticParser` from a ParGlare grammar file'''
        return cls.from_grammar(parglare.Grammar.from_file(f), **kwargs)

    def parse(self, data: str, **kwargs) -> typing.Any:
        '''Parses some string data (passes it to the underlying `parser.parse()`)'''
        return self.parser.parse(data, **kwargs)
