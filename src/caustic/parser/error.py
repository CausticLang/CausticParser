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

def format_pre_context(l: parglare.common.Location, lines: list[str], llen: int) -> cabc.Generator[str, None, None]:
    yield f'{(l.line-1):0{llen}} {lines[l.line-2]}\n'
def format_post_context(l: parglare.common.Location, lines: list[str], llen: int) -> cabc.Generator[str, None, None]:
    yield f'\n{(l.line_end+1):0{llen}} {lines[l.line_end]}'
def format_inner_context(l: parglare.common.Location, lines: list[str], llen: int) -> cabc.Generator[str, None, None]:
    if l.line == l.line_end:
        yield f'{l.line:0{llen}} {lines[l.line-1]}\n'
        yield f'{" "*llen} {" "*(l.column)}^'
        if l.column != l.column_end:
            yield f'{"~"*(l.column_end-l.column-1)}^'
        return
    yield f'{" "*llen} {" "*(l.column-1)}v\n'
    for ln in range(l.line, l.line_end+1):
        yield f'{ln:0{llen}} {lines[ln-1]}\n'
    yield f'{" "*llen} {" "*(l.column_end)}^'

def format_context(l: parglare.common.Location,
                   precontext: bool = True, postcontext: bool = True) -> cabc.Generator[str, None, None]:
    lines = l.input_str.split('\n')
    llen = len(str(l.line_end))
    if precontext and (l.line > 1):
        yield from format_pre_context(l, lines, llen)
    yield from format_inner_context(l, lines, llen)
    if postcontext and (l.line_end+1 < len(lines)):
        yield from format_post_context(l, lines, llen)
    

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
