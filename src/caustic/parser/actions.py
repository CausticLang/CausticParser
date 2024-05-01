#!/bin/python3

'''Supplies some useful builtin ParGlare actions'''

#> Imports
import typing
from caustic import cst
from parglare import get_collector
#</Imports

#> Header >/
__all__ = ('action',
           'obj',
           'node',
           'pass_last', 'pass_meta')

action = get_collector()

# Override
@action
def obj(ctx, _, **kwargs) -> dict:
    '''
        Overrides the default ParGlare action for named matches,
            making them return dictionaries instead of objects
    '''
    return kwargs

# Node
@action
def node(ctx, _, **kwargs) -> cst.CSTNode:
    '''
        Constructs a node, using the "node" key in the production's
            user-metadata
        All of the production's metadata is also added to kwargs, except for `'node'`
    '''
    kwargs = {k: v for k,v in ctx.production.user_meta.items() if k != 'node'} | kwargs
    try:
        return cst.CSTNode.NODE_DIRECTORY[ctx.production.node](
            source=cst.SourceInfo.from_parglare_ctx(ctx), **kwargs)
    except Exception as e:
        e.add_note('Whilst constructing node from production: {ctx.production!r}')
        raise e
@action
def unpack_node(ctx, args, **kwargs) -> cst.CSTNode:
    '''
        Same as `node`, but also adds a dict from args
        By default, uses args[0], but this can be configured
            via the user-metadata key 'index'
    '''
    return node(ctx, (), **(args[ctx.production.user_meta.get('index', 0)] | kwargs))

# Utility
@action
def pass_last(ctx, args) -> typing.Any:
    '''Returns the last index in `args`'''
    return args[-1]
@action
def pass_meta(ctx, _, **kwargs) -> dict:
    '''Adds the production's user-metadata to kwargs and returns it'''
    return kwargs | ctx.production.user_meta
