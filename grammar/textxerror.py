#!/bin/python3

'''Provides `[i]format_exc()` for TextX error formatting'''

#> Imports
import textx
from collections.abc import Iterable, Generator
#</Imports

#> Header >/
__all__ = ('iformat_exc', 'format_exc')

def iformat_exc(e: textx.exceptions.TextXError, *, context: Iterable[str] | None = None,
                add_location: bool = True, add_message: bool = True, strmatch_context: bool = True,
                pre_context: bool = True, line: bool = True, here_context: bool = True, post_context: bool = True, line_pfx: bool = True,
                here_col: str = ' ', here_text: str = '^ HERE', sep_text: str = f'\n{"-"*80}\n') -> Generator[str, None, None]:
    '''
        Formats a TextX syntax error'
        Slightly more pleasing than the default formatting,
            in my opinion
        Yields strings, result should be joined on nothing ("")
    '''
    linesize = len(str(e.line+1)) + 1
    # Obtain better context
    if context is None:
        if e.filename is None:
            raise FileNotFoundError('Cannot format exception without filename when context is not given')
        with open(e.filename) as f:
            lines = f.readlines()
    else: lines = tuple(context)
    # Header
    if add_message:
        if add_location:
            yield f'{e.filename}[{e.line}:{e.col}]: '
        yield f'{e.message}'
    if strmatch_context and hasattr(e, 'expected_rules'):
        for r in e.expected_rules:
            if ((not isinstance(r, textx.lang.StrMatch))
                or isinstance(r.id, int)): continue
            yield f'\nNote: {r.id} = {r.to_match!r}'
    yield sep_text
    # Pre-context
    if pre_context and (e.line > 1):
        if line_pfx: yield f'{e.line-1:0{linesize}} '
        yield lines[e.line-2]
    # Error line
    if line:
        if line_pfx: yield f'{e.line:0{linesize}} '
        yield lines[e.line-1]
        if here_context:
            yield f'\n{(e.col+linesize)*here_col}{here_text}'
        yield '\n'
    # Post-context
    if post_context:
        if line_pfx: yield f'{e.line+1:0{linesize}} '
        yield lines[e.line+1] if e.line < len(lines) else '<EOF>'

def format_exc(e: textx.exceptions.TextXSyntaxError, **kwargs) -> str:
    '''
        Formats a TextX syntax error'
        Slightly more pleasing than the default formatting,
            in my opinion
        See `iformat_exc()` for kwargs
    '''
    return ''.join(iformat_exc(e, **kwargs))
