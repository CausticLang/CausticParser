#!/bin/python3

'''Loads a `.cagspec` file (see `load()`)'''

#> Imports
import textx
from pathlib import Path
from collections import abc as cabc

from .textxcompose import compose
#</Imports

#> Header >/
__all__ = ('get', 'compile', 'load')

def _resolve(f: Path, src: Path, include_dirs: cabc.Sequence[Path]) -> Path:
    if f.is_absolute(): return f
    if (src.parent/f).is_file(): return src.parent/f
    for d in include_dirs:
        if (d/f).is_file(): return d/f
    raise FileNotFoundError(f'Cannot resolve relative path {f}')
def _load(f: Path, include_dirs: cabc.Sequence[Path]) -> cabc.Generator[Path | str]:
    if not f.is_file(): raise FileNotFoundError(f)
    for l in f.read_text().splitlines():
        if (not l) or l.startswith('#'): continue # comment or empty
        if l.startswith('$'): # meta-include
            try: p = _resolve(Path(l.removeprefix('$')), f, include_dirs)
            except FileNotFoundError as e:
                e.add_note(f'Included from {f}')
                raise e
            yield from _load(p, include_dirs)
            continue
        if l.startswith('*'): # keyword
            yield l.removeprefix('*')
            continue
        yield _resolve(Path(l), f, include_dirs) # grammar source

def get(f: Path, *, include_dirs: cabc.Sequence[Path] = (Path('.'),)) -> tuple[str, frozenset[str]]:
    '''
        Fetches and preprocesses grammar, but does not compile it
        Note: `load()` has better error messages in cases of grammar errors
    '''
    keywords = set()
    parts = []
    for ps in _load(f, include_dirs):
        if isinstance(ps, str): keywords.add(ps)
        else: parts.append(ps)
    return (compose(*parts, no_gen_model=True), frozenset(keywords))

def compile(parts: str, keywords: frozenset[str], **kwargs) -> textx.metamodel.TextXMetaModel:
    '''
        Compiles grammar from parts previously obtained from `get()`
        Note: `load()` has better error messages in cases of grammar errors
    '''
    return textx.metamodel_from_str(parts, **kwargs)

def load(f: Path, *, include_dirs: cabc.Sequence[Path] = (Path('.'),), **kwargs) -> tuple[textx.metamodel.TextXMetaModel, frozenset[str]]:
    '''Loads a `.cagspec` file'''
    keywords = set()
    parts = []
    for ps in _load(f, include_dirs):
        if isinstance(ps, str): keywords.add(ps)
        else: parts.append(ps)
    return (compose(*parts, **kwargs), frozenset(keywords)) # inject=f'Keyword: {"|".join(map(repr, keywords))} ;'
