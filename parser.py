#!/bin/python3

'''Provides the `CausticParser` class'''

#> Imports
import typing
import parglare
from pathlib import Path

from loader import load as metaload
#</Imports

#> Header >/
__all__ = ('CausticParser',)

class CausticParser:
    '''Parses Caustic source code into a CST'''
    __slots__ = ('parser',)

    parser: parglare.Parser

    def __init__(self, parser: parglare.Parser):
        self.parser = parser
    @classmethod
    def from_grammar(cls, grammar: parglare.Grammar, *,
                     parser_type: type[parglare.Parser] = parglare.Parser, **parser_kwargs) -> typing.Self:
        '''Creates a `CausticParser` from a grammar'''
        return cls(parser_type(grammar, **parser_kwargs))
    @classmethod
    def from_cagspec(cls, cspec: Path, **kwargs):
        '''Creates a `CausticParser` from a Caustic Grammar Spec file'''
        return cls.from_grammar(metaload(cspec), **kwargs)


# test
try:
    p = CausticParser.from_cagspec(Path('../CausticGrammar/default.cagspec'))
except Exception as ee:
    e = ee
    raise
