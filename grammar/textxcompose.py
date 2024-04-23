#!/bin/python3

'''
    Uses some hacks to compose a TextX metamodel
        from multiple files
    See `compose()`
'''

#> Imports
import io
import textx
from pathlib import Path
#</Imports

#> Header >/
__all__ = ('compose',)

def compose(*files: Path, hack_exc: bool = True, inject: str | None = None,
            no_gen_model: bool = False, **kwargs) -> textx.metamodel.TextXMetaModel | str:
    '''
        Composes multiple files into a single TextX metamodel
        If `hack_exc`, then a `TextXSyntaxError` will have its
            `.line` and `.filename` attributes changed to the corresponding
            source for easier debugging
    '''
    lines = 0
    source_info = {}
    with io.StringIO() as sio:
        for f in files:
            txt = f.read_text()
            sio.write(txt)
            sio.write('\n')
            lines += txt.count('\n') + 1
            source_info[lines] = f
        if inject is not None:
            sio.write(inject)
            sio.write('\n')
            lines += txt.count('\n') + 1
            source_info[lines] = '<textxcompose injected>'
        composed = sio.getvalue()
    if no_gen_model: return composed
    try:
        return textx.metamodel_from_str(composed, **kwargs)
    except textx.exceptions.TextXSyntaxError as txse:
        if not hack_exc: raise txse
        last = 0
        for sl,sn in source_info.items():
            if txse.line > sl:
                last = sl
                continue
            txse.line = txse.line - last
            txse.filename = sn
            break
        else:
            txse.line = txse.line - last
            txse.filename = sn
        raise txse
