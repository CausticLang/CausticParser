#!/bin/python3

'''Provides the `CausticParser` class'''

#> Imports
import os
import parglare
import tempfile
import contextlib
from pathlib import Path
#</Imports

__all__ = ('actions', 'cli', 'error',
           'builtin_grammar', 'compose')

#> Package
from . import actions
# cli is not imported here due to click being an optional dependency
from . import error
#</Package

#> Header >/
def builtin_grammar(sub: os.PathLike | str | None = None, *, check: bool = True) -> Path:
    '''
        Attempts to fetch the builtin grammar base path
        If a string or path is given, it is added to the end of the path
        If `check` is true, the path is checked to ensure it exists
    '''
    try: from caustic import grammar
    except Exception as e:
        e.add_note('Whilst attempting to fetch builtin grammar')
        raise e
    gp = Path(grammar.__path__[0])
    if sub is not None: gp /= sub
    if check and not gp.exists():
        e = FileNotFoundError(f'File or directory {gp} does not exist')
        e.add_note('Whilst attempting to fetch builtin grammar')
        raise e
    return gp

class compose(tempfile.TemporaryDirectory):
    '''
        Creates a temporary file containing imports for all of the `files`,
            loads it, and then removes it
        `start_symbol` is the contents of the starting rule,
            the easiest usage is to simply input the FQN of the starting rule
        Note that this is *extremely* slow **every time**, as
            the precompiled table is removed whenever this context manager exits
    '''
    __slots__ = ('files', 'start_rule', 'kwargs')

    def __init__(self, *files: Path, start_rule: str, **kwargs) -> parglare.Grammar:
        super().__init__(prefix='caustic.parser.compose')
        self.files = files
        self.start_rule = start_rule
        self.kwargs = kwargs
    def __enter__(self) -> parglare.Grammar:
        name = super().__enter__()
        cb = Path(name, 'compose_base.pg')
        with cb.open('w') as cbf:
            for f in self.files:
                cbf.write(f'import {f.absolute().as_posix()!r} ;\n')
            cbf.write(f'_: {self.start_rule} ;')
        return parglare.Grammar.from_file(cb, **self.kwargs)
