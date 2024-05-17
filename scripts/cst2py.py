#!/bin/python3

'''Utility module to convert serialized CSTs to Python classes'''

#> Imports
from enum import IntEnum
#</Imports

#> Header >/
__all__ = ('NodeType',)

class NodeType(IntEnum):
    '''cst_NodeType'''
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
