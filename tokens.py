#!/bin/python3

'''Supplies `Token` and `TOKEN_WIDTH`'''

#> Imports
from enum import IntEnum
#</Imports

#> Header >/
__all__ = ('TOKEN_WIDTH', 'Token')

TOKEN_WIDTH = 1 # bytes per token

class Token(IntEnum):
    '''Supplies standard tokens for Caustic'''

    def __repr__(self) -> str:
        return f'<{type(self).__qualname__}.{self.name}: 0x{self.value:0{TOKEN_WIDTH*2}X}>'
    def __str__(self) -> str:
        return f'0x{self.value:0{TOKEN_WIDTH*2}X} <{self.name}>'

    EOF = 2**(8*TOKEN_WIDTH)-1 # 0xFF
    NOP = 0x01

    # Atoms - 0x11-0x1F
    IDENTIFIER  = 0x11
    LITERAL_INT = 0x12
    LITERAL_STR = 0x13

    # Operators - 0x21-0x4F
    ## Unary
    OP_UPLUS    = 0x21
    OP_UMINUS   = 0x22
    OP_BITNOT   = 0x23
    OP_LOGNOT   = 0x24
    ## Binary
    OP_ASSIGNS  = 0x31 # assignment statement
    OP_ASSIGNE  = 0x32 # assignment expr
    OP_ADD      = 0x33
    OP_SUB      = 0x34
    OP_MULT     = 0x35
    OP_DIV      = 0x36
    OP_MOD      = 0x37
    OP_EXP      = 0x38
    OP_EQ       = 0x39
    OP_NE       = 0x3A
    OP_GT       = 0x3B
    OP_LT       = 0x3C
    OP_GE       = 0x3D
    OP_LE       = 0x3E
    OP_NULLISH  = 0x3F
    OP_LOGOR    = 0x30
    OP_LOGAND   = 0x31
    OP_LOGXOR   = 0x32
    OP_BITOR    = 0x33
    OP_BITXOR   = 0x34
    OP_LSHIFT   = 0x35
    OP_RSHIFT   = 0x36
    ## Ternary
    OP_TERNARY  = 0x4A

assert not any(t > Token.EOF for t in Token)
