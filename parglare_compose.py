#!/bin/python3

'''
    Uses some hacks to compose a Parglare metamodel
        from multiple files without dealing with namespacing
    See `compose()`
'''

#> Imports
import io
import parglare
from pathlib import Path
#</Imports

#> Header >/
__all__ = ('compose',)

def compose(*files: Path, hack_exc: bool = True, note_exc: bool = True,
            inject: str | None = None, no_gen_model: bool = False, **kwargs) -> parglare.Grammar | str:
    '''
        Composes multiple files into a single ParGlare grammar
        If `hack_exc`, then errors will have their line and filename
            changed to the proper source
        If `note_exc`, then errors will have a note attached
            with the filename and location
    '''
    lines = 0
    source_info = {}
    with io.StringIO() as sio:
        for f in files:
            txt = f.read_text()
            sio.write(txt)
            sio.write('\n')
            source_info[lines] = f
            lines += txt.count('\n') + 1
        if inject is not None:
            sio.write(inject)
            sio.write('\n')
            source_info[lines] = '<parglare_compose injected>'
            lines += txt.count('\n') + 1
        composed = sio.getvalue()
    if no_gen_model: return composed
    try:
        return parglare.Grammar.from_string(composed, **kwargs)
    except parglare.ParseError as pe:
        if not (hack_exc or note_exc): raise pe
        last = 0; name = None
        for sl,sn in source_info.items():
            if pe.location.line <= sl: break
            last = sl
            name = sn
        line = pe.location._line - last
        if hack_exc:
            pe.location._line = last
            if pe.location._line_end is not None:
                pe.location._line_end -= last
            pe.location.file_name = name
        if note_exc: pe.add_note(f'Composed, real line {line} of file {name}')
        raise pe
