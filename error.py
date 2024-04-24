#!/bin/python3

'''
    Better formatting for error messages
        (in my opinion)
'''

#> Imports
import parglare
from collections import abc as cabc
#</Imports

#> Header >/
__all__ = ('format_exc', 'iformat_exc')

def format_recognizer(r: parglare.grammar.Recognizer) -> str:
    match r:
        case parglare.grammar.RegExRecognizer():
            return f'/{r.regex.pattern}/'
        case parglare.grammar.StringRecognizer():
            return repr(r.value_cmp)
        case _:
            return repr(r)

def iformat_exc(e: parglare.ParseError, *,
                verbose_got: bool = True) -> cabc.Generator[str, None, None]:
    '''
        Formats a parsing error

        If `verbose_got` is true, then not only will the next token's value
            be printed, but additionally all of the names of the symbols
            that match it will be printed as well
    '''
    yield (f'Parse error occured at '
           f'{e.location.file_name or "<unknown>"}'
           f':{e.location.line}:{e.location.column}')

    yield '\nExpected: '
    if e.symbols_expected:
        for t in e.symbols_expected:
            yield f'\n{t.name} -> {format_recognizer(t.recognizer)}'
    else: yield ' <EOF>'

    yield '\nGot: '
    if e.tokens_ahead:
        yield f'{e.tokens_ahead[0].value!r}'
        if verbose_got:
            for t in e.tokens_ahead:
                yield f'\n- AKA: {t.symbol.name}'
    else:
        yield ' <EOF>'


def format_exc(e: parglare.ParseError, **kwargs) -> str:
    '''
        Formats a parsing error
        See `iformat_exc()` for kwargs
    '''
    return ''.join(iformat_exc(e, **kwargs))
