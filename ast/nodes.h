#ifndef cst_NODES_GUARD
#define cst_NODES_GUARD 1

#include <stddef.h>
#include <stdbool.h>

typedef unsigned int cst_index;

enum cst_NodeType {
    _NULL = 0, // illegal node
    // Expressions
    /// Atoms
    Identifier = 1,
    Bool = 2,
    Integer = 3,
    Float = 4,
    Char = 5,
    Bytes = 6,
    String = 7,
    /// Operations
    UnaryOp = 8,
    BinaryOp = 9,
    TernaryOp = 10,
    /// Accesses
    Attribute = 11,
    Subscript = 12,
    /// Procedures
    ProcInvoke = 13,
    ProcExpr = 14,
    // Statements
    ProcStmt = 15,
};

struct cst_NodeBase {
    enum cst_NodeType type;
    bool is_freed;
};

typedef struct cst_NodeBase** cst_node_vec;

struct cst_Root {
    cst_node_vec nodes;
};

#define cst_MKNODETYPE(name, members, body, ...) \
    struct cst_n##name { \
        struct cst_NodeBase _base; \
        members \
    }; \
    struct cst_n##name* cst_ninit_##name(struct cst_n##name* n, __VA_ARGS__) { cst_NODEDOWNCAST(n)->type = name; body; return n; }

#define cst_MKNODETYPE_S(name, mtype, mname) \
    cst_MKNODETYPE(name, mtype mname;, {n->mname = mname;}, mtype mname)

#define cst_NODECAST(type, node) ((struct cst_n##type*)node)
#define cst_NODEDOWNCAST(node) ((struct cst_NodeBase*)node)


#include "nodes/atoms.h"
#include "nodes/access.h"
#include "nodes/operators.h"
#include "nodes/procedures.h"


#endif
