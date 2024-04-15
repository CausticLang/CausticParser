#!/bin/python3

'''Generates tokens from lexed source'''

#> Imports
import typing as _typing
from collections import abc as _cabc

from tokens import Token as _Token
#</Imports

#> Header >/
#__all__ = () # too much work to fill right now

type _tgen = _cabc.Generator[_Token | _typing.Any, None, None]

def gentok_eof() -> _tgen: yield _Token.EOF
def gentok_nop() -> _tgen: yield _Token.NOP

def gentok_from_expression(type: _typing.Literal[b'atom', b'operator'], val: _typing.Any) -> _tgen:
    match type:
        case b'atom': yield from gentok_from_atom(**val)
        case b'operator': yield from gentok_from_operator(**val)
        case _: raise TypeError(f'Unknown expression type: {type!r}')

# Atoms
def gentok_from_atom(type: _typing.Literal[b'literal', b'identifier'], val: _typing.Any) -> _tgen:
    match type:
        case b'literal': yield from gentok_from_literal(**val)
        case b'identifier': yield from gentok_identifier(val)
        case _: raise TypeError(f'Unknown atom type: {type!r}')

def gentok_identifier(val: bytes) -> _tgen:
    yield _Token.IDENTIFIER
    yield name

## Literals
def gentok_from_literal(type: _typing.Literal[b'int', b'str'], **kwargs) -> _tgen:
    match type:
        case b'int': yield from gentok_literal_int(**kwargs)
        case b'str': yield from gentok_literal_str(**kwargs)
        case _: raise TypeError(f'Unknown literal type: {type!r}')

def gentok_literal_int(base: bytes, val: bytes) -> _tgen:
    yield _Token.LITERAL_INT
    yield int(val, base=int(base))
def gentok_literal_str(raw: bytes, val: bytes) -> _tgen:
    yield _Token.LITERAL_STR
    yield bool(raw)
    yield val

# Operators
_OP_TOKEN_TYPES = ('uplus', 'uminus', 'bitnot', 'lognot',
                   'assignstmt', 'assignexpr',
                   'add', 'sub', 'mult', 'div', 'mod', 'exp',
                   'eq', 'ne', 'gt', 'lt', 'ge', 'le', 'nullish',
                   'logor', 'logand', 'logxor',
                   'bitor', 'bitand', 'bitxor', 'lshift', 'rshift',
                   'ternary')
def gentok_from_operator(type: bytes, **kwargs) -> _tgen:
    dtype = type.decode(errors='replace')
    if dtype not in _OP_TOKEN_TYPES:
        raise TypeError(f'Unknown operator type: {type!r}')
    yield from globals()[f'gentok_op_{dtype}'](**kwargs)

## Unary
def gentok_op_uplus(target: dict) -> _tgen:
    yield _Token.OP_UPLUS
    yield from gentok_from_expression(**target)
def gentok_op_uminus(target: dict) -> _tgen:
    yield _Token.OP_UMINUS
    yield from gentok_from_expression(**target)
def gentok_op_bitnot(target: dict) -> _tgen:
    yield _Token.OP_BITNOT
    yield from gentok_from_expression(**target)
def gentok_op_lognot(target: dict) -> _tgen:
    yield _Token.OP_LOGNOT
    yield from gentok_from_expression(**target)
## Binary
### Assignment
def gentok_op_assignstmt(dst: dict, src: dict) -> _tgen:
    yield _Token.OP_ASSIGNS
    match dst['type']:
        case b'identifier': yield from gentok_identifier(dst['val'])
        case _: raise TypeError(f'Unknown assign-statement destination type: {dst["type"]!r}')
def gentok_op_assignexpr(dst: dict, src: dict) -> _tgen:
    yield _Token.OP_ASSIGNE
    yield from gentok_identifier(**dst)
    yield from gentok_from_expression(**src)
### Arithmetic
def gentok_op_add(left: dict, right: dict) -> _tgen:
    yield _Token.OP_ADD
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
def gentok_op_sub(left: dict, right: dict) -> _tgen:
    yield _Token.OP_SUB
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
def gentok_op_mult(left: dict, right: dict) -> _tgen:
    yield _Token.OP_MULT
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
def gentok_op_div(left: dict, right: dict) -> _tgen:
    yield _Token.OP_DIV
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
def gentok_op_mod(left: dict, right: dict) -> _tgen:
    yield _Token.OP_MOD
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
def gentok_op_exp(left: dict, right: dict) -> _tgen:
    yield _Token.OP_EXP
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
### Comparison
def gentok_op_eq(left: dict, right: dict) -> _tgen:
    yield _Token.OP_EQ
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
def gentok_op_ne(left: dict, right: dict) -> _tgen:
    yield _Token.OP_NE
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
def gentok_op_gt(left: dict, right: dict) -> _tgen:
    yield _Token.OP_GT
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
def gentok_op_lt(left: dict, right: dict) -> _tgen:
    yield _Token.OP_LT
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
def gentok_op_ge(left: dict, right: dict) -> _tgen:
    yield _Token.OP_GE
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
def gentok_op_le(left: dict, right: dict) -> _tgen:
    yield _Token.OP_LE
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
def gentok_op_nullish(left: dict, right: dict) -> _tgen:
    yield _Token.OP_NULLISH
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
### Logical
def gentok_op_logor(left: dict, right: dict) -> _tgen:
    yield _Token.OP_LOGOR
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
def gentok_op_logand(left: dict, right: dict) -> _tgen:
    yield _Token.OP_LOGAND
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
def gentok_op_logxor(left: dict, right: dict) -> _tgen:
    yield _Token.OP_LOGXOR
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
### Bitwise
def gentok_op_bitor(left: dict, right: dict) -> _tgen:
    yield _Token.OP_BITOR
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
def gentok_op_bitand(left: dict, right: dict) -> _tgen:
    yield _Token.OP_BITAND
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
def gentok_op_bitxor(left: dict, right: dict) -> _tgen:
    yield _Token.OP_BITXOR
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
def gentok_op_lshift(left: dict, right: dict) -> _tgen:
    yield _Token.OP_LSHIFT
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
def gentok_op_rshift(left: dict, right: dict) -> _tgen:
    yield _Token.OP_RSHIFT
    yield from gentok_from_expression(**left)
    yield from gentok_from_expression(**right)
## Ternary
def gentok_op_ternary(cmp: dict, true: dict, false: dict) -> _tgen:
    yield _Token.OP_TERNARY
    yield from gentok_from_expression(**cmp)
    yield from gentok_from_expression(**true)
    yield from gentok_from_expression(**false)
