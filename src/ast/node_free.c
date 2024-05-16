#include <assert.h>
#include <stdbool.h>
#include <malloc.h>

#include "cst.h"

void cst_free_node(struct cst_NodeBase* node) {
    assert(!node->is_freed);
    switch (node->type) {
        #define _cst_FREEVAL(type) case type: free(cst_NODECAST(type, node)->val); break
        case Entrypoint:
        case Bool:
        case UnaryOp: case BinaryOp: case TernaryOp:
        case Attribute: case Subscript:
        case IfStmt: case ElIfStmt: case ElseStmt: case ForStmt: case WhileStmt:
            break; // no freeing needed
        case Block:
            free(cst_NODECAST(Block, node)->nodes);
            break;
        _cst_FREEVAL(Identifier);
        _cst_FREEVAL(Integer);
        case Float:
            free(cst_NODECAST(Float, node)->integer);
            free(cst_NODECAST(Float, node)->fractional);
            break;
        _cst_FREEVAL(Char);
        _cst_FREEVAL(Bytes);
        _cst_FREEVAL(String);
        case ProcInvoke:
            free(cst_NODECAST(ProcInvoke, node)->args);
            for (int i = 0; i < cst_NODECAST(ProcInvoke, node)->kwarglen; i++)
                free(cst_NODECAST(ProcInvoke, node)->kwargs[i]);
            free(cst_NODECAST(ProcInvoke, node)->kwargs);
            break;
        case ProcExpr:
            free(cst_NODECAST(ProcExpr, node)->params);
            break;
        case ProcStmt:
            free(cst_NODECAST(ProcStmt, node)->params);
            break;
        default: assert(false);
        #undef _cst_FREEVAL
    }
    node->is_freed = true;
}
