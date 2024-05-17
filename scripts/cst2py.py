#!/bin/python3

'''Utility module to convert serialized CSTs to Python classes'''

#> Imports
import struct
import typing
from enum import IntEnum
#</Imports

#> Header >/
__all__ = ('NodeType',)

class NodeType(IntEnum):
    '''cst_NodeType'''
    __slots__ = ()

    # Control
    Entrypoint = 0
    ExtraData = 255
    Block = 16
    # Types
    Type = 24
    # Expressions
    ## Atoms
    Identifier = 1
    Bool = 2
    Integer = 3
    Float = 4
    Char = 5
    Bytes = 6
    String = 7
    ## Operations
    UnaryOp = 8
    BinaryOp = 9
    TernaryOp = 10
    ## Accesses
    Attribute = 11
    Subscript = 12
    ## Procedures
    ProcInvoke = 13
    ProcExpr = 14
    # Statements
    ProcStmt = 15
    IfStmt = 17
    ElIfStmt = 18
    ElseStmt = 19
    ForStmt = 20
    WhileStmt = 21
    PassStmt = 22
    FlowControlStmt = 23
    Declaration = 25
    Assignment = 26

    def to_char(self) -> bytes:
        return bytes((self.value,))
    def serialize(self, stream: typing.BinaryIO) -> None:
        stream.write(self.STRUCT.pack(self.to_char()))
    @classmethod
    def deserialize(cls, stream: typing.BinaryIO) -> typing.Self:
        return cls(cls.STRUCT.unpack(stream.read(cls.STRUCT.size))[0][0])
NodeType.STRUCT = struct.Struct('c')
