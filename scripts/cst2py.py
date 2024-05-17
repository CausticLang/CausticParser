#!/bin/python3

# ! WIP !

'''Utility module to convert serialized CSTs to Python classes'''

#> Imports
import struct
import typing
from enum import IntEnum
from types import SimpleNamespace
#</Imports

#> Header >/
__all__ = ('Serializers', 'Deserializers', 'NodeType')

# (De)serializers
Serializers = SimpleNamespace()
Deserializers = SimpleNamespace()

_BOOL_STRUCT = struct.Struct('?')
def _serialize_bool(b: bool) -> bytes:
    return _BOOL_STRUCT.pack(b)
Serializers.bool = _serialize_bool
def _deserialize_bool(d: bytes) -> bool:
    return _BOOL_STRUCT.unpack(d)[0]
Deserializers.bool = _deserialize_bool
_INT_STRUCT = struct.Struct('2s')
def _serialize_int(i: int) -> bytes:
    return _INT_STRUCT.pack(i.to_bytes(size=2, signed=True))
Serializers.int = _serialize_int
def _deserialize_int(d: bytes) -> int:
    int.from_bytes(_INT_STRUCT.unpack(d)[0], signed=True)
Deserializers.int = _deserialize_int
_UINT_STRUCT = struct.Struct('2s')
def _serialize_uint(u: int) -> bytes:
    return _INT_STRUCT.pack(u.to_bytes(size=2, signed=False))
Serializers.int = _serialize_int
def _deserialize_uint(d: bytes) -> int:
    int.from_bytes(_INT_STRUCT.unpack(d)[0], signed=False)
Deserializers.uint = _deserialize_uint
Serializers.sizet = Serializers.index = Serializers.uint
Deserializers.sizet = Deserializers.index = Deserializers.uint
_PROCKWARG_STRUCT = struct.Struct('2s2s')
def _serialize_prockwarg(k: int, v: int) -> bytes:
    return _PROCKWARGS_STRUCT.pack(k.to_bytes(size=2, signed=False), v.to_bytes(size=2, signed=False))
Serializers.prockwarg = _serialize_prockwarg
def _deserialize_porckwarg(d: bytes) -> tuple[int, int]:
    k,v = _PROCKWARGS_STRUCT.unpack(d)
    return (int.from_bytes(k, signed=False), int.from_bytes(v, signed=False))
Deserializers.prockwarg = _deserialize_prockwarg
_PROCPARAM_STRUCT = struct.Struct(

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

class Entrypoint(typing.NamedTuple):
    STRUCT: typing.ClassVar = struct.Struct(
