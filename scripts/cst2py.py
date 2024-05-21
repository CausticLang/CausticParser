#!/bin/python3

'''Utility module to convert serialized CSTs to Python classes'''

#> Imports
import struct
import typing
from enum import IntEnum
from types import SimpleNamespace
from collections import abc as cabc
#</Imports

#> Header >/
def deserialize_nodes(stream: typing.BinaryIO) -> cabc.Generator[typing.NamedTuple, None, None]:
    while (c := stream.read(1)):
        assert c == b'\1'
        base = BaseNode.deserialize(stream)
        if base.type.name not in globals():
            raise NotImplementedError(f'Unhandled node type {base.type!r}')
        yield globals()[base.type.name].deserialize(base, stream)

def _deserialize_string(stream: typing.BinaryIO) -> cabc.Generator[bytes, None, None]:
    while (c := stream.read(1)) != b'\0': yield c
def deserialize_string(stream: typing.BinaryIO) -> bytes:
    return b''.join(_deserialize_string(stream))
def serialize_string(stream: typing.BinaryIO, string: bytes) -> None:
    stream.write(string + b'\0')

class NodeType(IntEnum):
    '''cst_NodeType'''
    __slots__ = ()

    # Control
    Entrypoint = 0
    ExtraData = 255
    Block = 16
    # Types
    Type = 24
    Enum = 27
    Struct = 28
    StructEnum = 29,
    Class = 30,
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

    def serialize(self, stream: typing.BinaryIO) -> None:
        stream.write(bytes((self.value,)))
    @classmethod
    def deserialize(cls, stream: typing.BinaryIO) -> typing.Self:
        return cls(stream.read(1)[0])

class BaseNode(typing.NamedTuple):
    type: NodeType
    pos_start: int
    pos_end: int
    lineno: int
    colno: int

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.type.serialize(stream)
        stream.write(self.pos_start.to_bytes(2, byteorder='little'))
        stream.write(self.pos_end.to_bytes(2, byteorder='little'))
        stream.write(self.lineno.to_bytes(2, byteorder='little'))
        stream.write(self.colno.to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, stream: typing.BinaryIO) -> typing.Self:
        return cls(NodeType.deserialize(stream),
                   int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'))


class Entrypoint(typing.NamedTuple):
    base: BaseNode
    eof_pos: int
    node: int

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.eof_pos.to_bytes(2, byteorder='little'))
        stream.write(self.node.to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base,
                   int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'))
class ExtraData(typing.NamedTuple):
    base: BaseNode
    meta: bytes
    data: bytes

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        serialize_string(stream, meta)
        serialize_string(stream, data)
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> None:
        return cls(base,
                   deserialize_string(stream),
                   deserialize_string(stream))
class Block(typing.NamedTuple):
    base: BaseNode
    node_count: int
    nodes: list[int]

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.node_count.to_bytes(2, byteorder='little'))
        for n in self.nodes:
            stream.write(n.to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> None:
        ncount = int.from_bytes(stream.read(2), byteorder='little')
        return cls(base, ncount,
                   [int.from_bytes(stream.read(2), byteorder='little') for _ in range(ncount)])

class Type(typing.NamedTuple):
    base: BaseNode
    top: int
    sub: int

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.top.to_bytes(2, byteorder='little'))
        stream.write(self.sub.to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base,
                   int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'))
class Enum(typing.NamedTuple):
    base: BaseNode
    name: int
    base_: int
    member_count: int
    members: list[tuple[int, int]]

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.name.to_bytes(2, byteorder='little'))
        stream.write(self.base_.to_bytes(2, byteorder='little'))
        stream.write(self.member_count.to_bytes(2, byteorder='little'))
        for ma,mb in self.members:
            stream.write(ma.to_bytes(2, byteorder='little'))
            stream.write(mb.to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        name = int.from_bytes(stream.read(2), byteorder='little')
        base_ = int.from_bytes(stream.read(2), byteorder='little')
        member_count = int.from_bytes(stream.read(2), byteorder='little')
        return cls(base, name, base_, member_count,
                   [(int.from_bytes(stream.read(2), byteorder='little'),
                     int.from_bytes(stream.read(2), byteorder='little')) for _ in range(member_count)])
class Struct(typing.NamedTuple):
    base: BaseNode
    name: int
    base_: int
    member_count: int
    members: list[tuple[int, int, int]]

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.name.to_bytes(2, byteorder='little'))
        stream.write(self.base_.to_bytes(2, byteorder='little'))
        stream.write(self.member_count.to_bytes(2, byteorder='little'))
        for ma,mb,mc in self.members:
            stream.write(ma.to_bytes(2, byteorder='little'))
            stream.write(mb.to_bytes(2, byteorder='little'))
            stream.write(mc.to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        name = int.from_bytes(stream.read(2), byteorder='little')
        base_ = int.from_bytes(stream.read(2), byteorder='little')
        member_count = int.from_bytes(stream.read(2), byteorder='little')
        return cls(base, name, base_, member_count,
                   [(int.from_bytes(stream.read(2), byteorder='little'),
                     int.from_bytes(stream.read(2), byteorder='little'),
                     int.from_bytes(stream.read(2), byteorder='little')) for _ in range(member_count)])
class StructEnum(typing.NamedTuple):
    base: BaseNode
    name: int
    base_: int
    member_count: int
    members: list[int]

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.name.to_bytes(2, byteorder='little'))
        stream.write(self.base_.to_bytes(2, byteorder='little'))
        stream.write(self.member_count.to_bytes(2, byteorder='little'))
        for m in self.members:
            stream.write(m.to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        name = int.from_bytes(stream.read(2), byteorder='little')
        base_ = int.from_bytes(stream.read(2), byteorder='little')
        member_count = int.from_bytes(stream.read(2), byteorder='little')
        return cls(base, name, base_, member_count,
                   [int.from_bytes(stream.read(2), byteorder='little') for _ in range(member_count)])
class Class(typing.NamedTuple):
    base: BaseNode
    name: int
    base_: int
    member_count: int
    members: list[dict]

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.name.to_bytes(2, byteorder='little'))
        stream.write(self.base_.to_bytes(2, byteorder='little'))
        stream.write(self.member_count.to_bytes(2, byteorder='little'))
        for m in self.members:
            stream.write((m['is_static'] << 1 | m['is_method']).to_bytes(1, byteorder='little'))
            if m['is_method']:
                stream.write(m['method'].to_bytes(2, byteorder='little'))
            else:
                stream.write(m['member'][0].to_bytes(2, byteorder='little'))
                stream.write(m['member'][1].to_bytes(2, byteorder='little'))
                stream.write(m['member'][2].to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        self = cls(base, int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'), [])
        for _ in range(self.member_count):
            self.members.append({})
            props = int.from_bytes(stream.read(1), byteorder='little')
            self.members[-1]['is_static'] = bool(props & 0b10)
            self.members[-1]['is_method'] = bool(props & 0b01)
            if props & 0b01:
                self.members[-1]['method'] = int.from_bytes(stream.read(2), byteorder='little')
            else:
                self.members[-1]['member'] = (int.from_bytes(stream.read(2), byteorder='little'),
                                              int.from_bytes(stream.read(2), byteorder='little'),
                                              int.from_bytes(stream.read(2), byteorder='little'))
        return self

class Identifier(typing.NamedTuple):
    base: BaseNode
    val: bytes

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        serialize_string(stream, self.val)
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, deserialize_string(stream))
class Bool(typing.NamedTuple):
    base: BaseNode
    val: bool

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.val.to_bytes(1, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, bool.from_bytes(stream.read(1), byteorder='little'))
class Integer(typing.NamedTuple):
    base: BaseNode
    val: bytes
    base_: int

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        serialize_string(stream, self.val)
        stream.write(self.base_.to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, deserialize_string(stream), int.from_bytes(stream.read(2), byteorder='little'))
class Float(typing.NamedTuple):
    base: BaseNode
    integer: bytes
    fractional: bytes
    exp: int

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        serialize_string(stream, self.integer)
        serialize_string(stream, self.fractional)
        stream.write(self.base_.to_bytes(2, byteorder='little', signed=True))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, deserialize_string(stream), deserialize_string(stream),
                   int.from_bytes(stream.read(2), byteorder='little', signed=True))
class Char(typing.NamedTuple):
    base: BaseNode
    val: bytes

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        serialize_string(stream, self.val)
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, deserialize_string(stream))
class Bytes(typing.NamedTuple):
    base: BaseNode
    val: bytes
    raw: bool

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        serialize_string(stream, self.val)
        stream.write(self.raw.to_bytes(1, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, deserialize_string(stream), bool.from_bytes(stream.read(1), byteorder='little'))
class String(typing.NamedTuple):
    base: BaseNode
    val: bytes
    raw: bool

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        serialize_string(stream, self.val)
        stream.write(self.raw.to_bytes(1, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, deserialize_string(stream), bool.from_bytes(stream.read(1), byteorder='little'))


class OperatorType(IntEnum):
    # Unary
    POS = 1; NEG = 2; LOGINV = 3; BITINV = 4; INC = 5; DEC = 6
    # Binary
    ADD = 7; SUB = 8; MULT = 9; MMUL = 10; DIV = 11; MOD = 12; POW = 13
    EQ = 14; NE = 15; IS = 16; ISNT = 17; LT = 18; LE = 19; GT = 20; GE = 21
    NULLCOALESCING = 22
    LOGAND = 23; LOGOR = 24; LOGXOR = 25
    BITAND = 26; BITOR = 27; BITXOR = 28; LSHIFT = 29; RSHIFT = 30
    # Ternary
    CONDITIONAL = 31

class UnaryOp(typing.NamedTuple):
    base: BaseNode
    op: OperatorType
    target: int

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.op.to_bytes(1, byteorder='little'))
        stream.write(self.target.to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, OperatorType.from_bytes(stream.read(1), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'))
class BinaryOp(typing.NamedTuple):
    base: BaseNode
    op: OperatorType
    left: int
    right: int

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.op.to_bytes(1, byteorder='little'))
        stream.write(self.left.to_bytes(2, byteorder='little'))
        stream.write(self.right.to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, OperatorType.from_bytes(stream.read(1), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'))
class TernaryOp(typing.NamedTuple):
    base: BaseNode
    op: OperatorType
    a: int; b: int; c: int

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.op.to_bytes(1, byteorder='little'))
        stream.write(self.a.to_bytes(2, byteorder='little'))
        stream.write(self.b.to_bytes(2, byteorder='little'))
        stream.write(self.c.to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, OperatorType.from_bytes(stream.read(1), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'))

class Attribute(typing.NamedTuple):
    base: BaseNode
    top: int
    sub: int

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.top.to_bytes(2, byteorder='little'))
        stream.write(self.sub.to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'))
class Subscript(typing.NamedTuple):
    base: BaseNode
    top: int
    sub: int

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.top.to_bytes(2, byteorder='little'))
        stream.write(self.sub.to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'))

class ProcInvoke(typing.NamedTuple):
    base: BaseNode
    proc: int
    args: list[int]
    arglen: int
    kwargs: list[tuple[int, int]]
    kwarglen: int

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.proc.to_bytes(2, byteorder='little'))
        stream.write(self.arglen.to_bytes(2, byteorder='little'))
        for a in self.args:
            stream.write(a.to_bytes(2, byteorder='little'))
        stream.write(self.kwarglen.to_bytes(2, byteorder='little'))
        for k,v in self.kwargs:
            stream.write(k.to_bytes(2, byteorder='little'))
            stream.write(v.to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        proc = int.from_bytes(stream.read(2), byteorder='little')
        arglen = int.from_bytes(stream.read(2), byteorder='little')
        args = [int.from_bytes(stream.read(2), byteorder='little')
                for _ in range(arglen)]
        kwarglen = int.from_bytes(stream.read(2), byteorder='little')
        kwargs = [(int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'))
                  for _ in range(kwarglen)]
        return cls(base, proc, args, arglen, kwargs, kwarglen)

class ProcParamType(IntEnum):
    POS_ONLY = 1
    POS_OR_KW = 2
    KW_ONLY = 3
class _Proc_Base:
    __slots__ = ()

    def _serialize(self, stream: typing.BinaryIO) -> None:
        stream.write(self.rtype.to_bytes(2, byteorder='little'))
        stream.write(self.param_len.to_bytes(2, byteorder='little'))
        for p in self.params:
            stream.write(p['type'].to_bytes(2, byteorder='little'))
            stream.write(p['name'].to_bytes(2, byteorder='little'))
            stream.write(p['val'].to_bytes(2, byteorder='little'))
            stream.write(p['has_default'].to_bytes(1, byteorder='little'))
            stream.write(p['param_type'].to_bytes(1, byteorder='little'))
        stream.write(self.body.to_bytes(2, byteorder='little'))
    @staticmethod
    def _deserialize(stream: typing.BinaryIO) -> tuple[int, int, list[dict], int]:
        rtype = int.from_bytes(stream.read(2), byteorder='little')
        param_len = int.from_bytes(stream.read(2), byteorder='little')
        return (rtype,
                [{'type': int.from_bytes(stream.read(2), byteorder='little'),
                  'name': int.from_bytes(stream.read(2), byteorder='little'),
                  'val': int.from_bytes(stream.read(2), byteorder='little'),
                  'has_default': bool.from_bytes(stream.read(1), byteorder='little'),
                  'param_type': ProcParamType.from_bytes(stream.read(2), byteorder='little')}
                 for _ in range(param_len)], param_len, int.from_bytes(stream.read(2), byteorder='little'))
class ProcExpr(typing.NamedTuple):
    base: BaseNode
    rtype: int
    params: list[dict]
    param_len: int
    body: int

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        _Proc_Base._serialize(self, stream)
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, *_Proc_Base._deserialize(stream))
class ProcStmt(typing.NamedTuple):
    base: BaseNode
    name: int
    rtype: int
    params: list[dict]
    param_len: int
    body: int

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.name.to_bytes(2, byteorder='little'))
        _Proc_Base._serialize(self, stream)
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, int.from_bytes(stream.read(2), byteorder='little'), *_Proc_Base._deserialize(stream))

class _IfStmt_Base:
    __slots__ = ()

    @staticmethod
    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.condition.to_bytes(2, byteorder='little'))
        stream.write(self.body.to_bytes(2, byteorder='little'))
        stream.write(self.next.to_bytes(2, byteorder='little'))
    @staticmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'))
class IfStmt(typing.NamedTuple):
    base: BaseNode
    condition: int
    body: int
    next: int

    serialize = _IfStmt_Base.serialize
    deserialize = classmethod(_IfStmt_Base.deserialize)
class ElIfStmt(typing.NamedTuple):
    base: BaseNode
    condition: int
    body: int
    next: int

    serialize = _IfStmt_Base.serialize
    deserialize = classmethod(_IfStmt_Base.deserialize)
class ElseStmt(typing.NamedTuple):
    base: BaseNode
    body: int

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.body.to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, int.from_bytes(stream.read(2), byteorder='little'))
class ForStmt(typing.NamedTuple):
    base: BaseNode
    init: int
    cond: int
    iter: int
    body: int

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.init.to_bytes(2, byteorder='little'))
        stream.write(self.cond.to_bytes(2, byteorder='little'))
        stream.write(self.iter.to_bytes(2, byteorder='little'))
        stream.write(self.body.to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'))
class WhileStmt(typing.NamedTuple):
    base: BaseNode
    cond: int
    body: int
    do_while: bool

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.cond.to_bytes(2, byteorder='little'))
        stream.write(self.body.to_bytes(2, byteorder='little'))
        stream.write(self.do_while.to_bytes(1, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'),
                   bool.from_bytes(stream.read(1), byteorder='little'))
class PassStmt(typing.NamedTuple):
    base: BaseNode

    def serialize(self, stream: typing.BinaryIO) -> None: self.base.serialize(stream)
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self: return cls(base)

class FlowControlType(IntEnum):
    BREAK = 1; CONTINUE = 2

class FlowControlStmt(typing.NamedTuple):
    base: BaseNode
    type: FlowControlType

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.type.to_bytes(1, 'little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, FlowControlType.from_bytes(stream.read(1)))
class ReturnStmt(typing.NamedTuple):
    base: BaseNode
    val: int

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.val.to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, int.from_bytes(stream.read(2), byteorder='little'))
class Declaration(typing.NamedTuple):
    base: BaseNode
    type: int
    name: int
    val: int

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.type.to_bytes(2, byteorder='little'))
        stream.write(self.name.to_bytes(2, byteorder='little'))
        stream.write(self.val.to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'))
class Assignment(typing.NamedTuple):
    base: BaseNode
    target: int
    val: int

    def serialize(self, stream: typing.BinaryIO) -> None:
        self.base.serialize(stream)
        stream.write(self.target.to_bytes(2, byteorder='little'))
        stream.write(self.val.to_bytes(2, byteorder='little'))
    @classmethod
    def deserialize(cls, base: BaseNode, stream: typing.BinaryIO) -> typing.Self:
        return cls(base, int.from_bytes(stream.read(2), byteorder='little'),
                   int.from_bytes(stream.read(2), byteorder='little'))
