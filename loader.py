#!/bin/python3

'''Loads a Caustic Grammar Spec (`.cagspec`) file (see `load()`)'''

#> Imports
import parglare
from pathlib import Path
from collections import abc as cabc

from parglare_compose import compose
#</Imports

#> Header >/
__all__ = ('load',)

def _resolve(f: Path, src: Path, include_dirs: cabc.Sequence[Path]) -> Path:
    if f.is_absolute(): return f
    if (src.parent/f).is_file(): return src.parent/f
    for d in include_dirs:
        if (d/f).is_file(): return d/f
    raise FileNotFoundError(f'Cannot resolve relative path {f}')
def _load(f: Path, include_dirs: cabc.Sequence[Path]) -> cabc.Generator[Path]:
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
        yield _resolve(Path(l), f, include_dirs) # grammar source

def load(f: Path, *, include_dirs: cabc.Sequence[Path] = (Path('.'),), **kwargs) -> parglare.Grammar:
    '''Loads a `.cagspec` file'''
    return compose(*_load(f, include_dirs), **kwargs)
