enum cst_OperatorType {
    // Unary
    POS, NEG, LOGINV, BITINV, INC, DEC,
    // Binary
    ADD, SUB, MULT, MMUL, DIV, MOD, POW,
    EQ, NE, LT, LE, GT, GE,
    NULLCOALESCING,
    LOGAND, LOGOR, LOGXOR,
    BITAND, BITOR, BITXOR, LSHIFT, RSHIFT,
    // Ternary
    CONDITIONAL,
};


cst_MKNODETYPE(UnaryOp, enum cst_OperatorType op; cst_index target;, {
    n->op = op;
    n->target = target;
}, enum cst_OperatorType op, cst_index target);
cst_MKNODETYPE(BinaryOp, enum cst_OperatorType op; cst_index left; cst_index right;, {
    n->op = op;
    n->left = left;
    n->right = right;
}, enum cst_OperatorType op, cst_index left, cst_index right);
cst_MKNODETYPE(TernaryOp, enum cst_OperatorType op; cst_index a; cst_index b; cst_index c;, {
    n->op = op;
    n->a = a;
    n->b = b;
    n->c = c;
}, enum cst_OperatorType op, cst_index a, cst_index b, cst_index c);
