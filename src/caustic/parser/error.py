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

def format_pre_context(l: parglare.common.Location, l_start: int, l_end: int, c_start: int, c_end: int, lines: list[str], llen: int) -> cabc.Generator[str, None, None]:
    yield f'{(l_start-1):0{llen}} {lines[l_start-2]}\n'
def format_post_context(l: parglare.common.Location, l_start: int, l_end: int, c_start: int, c_end: int, lines: list[str], llen: int) -> cabc.Generator[str, None, None]:
    yield f'\n{(l_end+1):0{llen}} {lines[l_end]}'
def format_inner_context(l: parglare.common.Location, l_start: int, l_end: int, c_start: int, c_end: int, lines: list[str], llen: int) -> cabc.Generator[str, None, None]:
    if (l_start == l_end) or (l_end is None):
        yield f'{l_start:0{llen}} {lines[l_start-1]}\n'
        yield f'{" "*llen} {" "*(c_start)}^'
        if c_start != c_end:
            yield f'{"~"*(c_end-c_start-1)}^'
        return
    yield f'{" "*llen} {" "*(c_start-1)}v\n'
    for ln in range(l_start, l_end+1):
        yield f'{ln:0{llen}} {lines[ln-1]}\n'
    yield f'{" "*llen} {" "*(c_end)}^'

def format_context(l: parglare.common.Location,
                   precontext: bool = True, postcontext: bool = True) -> cabc.Generator[str, None, None]:
    lines = l.input_str.split('\n')
    l_start = l.line; l_end = l_start if l.line_end is None else l.line_end
    c_start = l.column; c_end = c_start if l.column_end is None else l.column_end
    llen = len(str(l_end))
    if precontext and (l_start > 1):
        yield from format_pre_context(l, l_start, l_end, c_start, c_end, lines, llen)
    yield from format_inner_context(l, l_start, l_end, c_start, c_end, lines, llen)
    if postcontext and (l_end+1 < len(lines)):
        yield from format_post_context(l, l_start, l_end, c_start, c_end, lines, llen)
    

def iformat_exc(e: parglare.ParseError, *, verbose_got: bool = False,
                precontext: bool = True, postcontext: bool = True) -> cabc.Generator[str, None, None]:
    '''
        Formats a parsing error

        If `verbose_got` is true, then not only will the next token's value
            be printed, but additionally all of the names of the symbols
            that match it will be printed as well
        If `precontext`/`postcontext` are true, then the line before/after the line(s) of
            the error will be embedded as well
    '''
    yield (f'Parse error occured at '
           f'{e.location.file_name or "<unknown>"}'
           f':{e.location.line}:{e.location.column}\n')

    yield f'{"-"*40}\n'
    yield from format_context(e.location, precontext, postcontext)
    yield f'\n{"-"*40}'

    yield '\nExpected:'
    if e.symbols_expected:
        for t in e.symbols_expected:
            yield f'\n {t.name} -> {format_recognizer(t.recognizer)}'
    else: yield ' <EOF>'

    yield '\nGot: '
    if e.tokens_ahead:
        yield f'{e.tokens_ahead[0].value!r}'
        if verbose_got:
            for t in e.tokens_ahead:
                yield f'\n- AKA: {t.symbol.name}'
    else:
        yield '<EOF>'


def format_exc(e: parglare.ParseError, **kwargs) -> str:
    '''
        Formats a parsing error
        See `iformat_exc()` for kwargs
    '''
    return ''.join(iformat_exc(e, **kwargs))
