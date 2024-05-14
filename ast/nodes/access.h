cst_MKNODETYPE(Attribute, cst_index top; cst_index sub;, {
    n->top = top;
    n->sub = sub;
}, cst_index top, cst_index sub);
cst_MKNODETYPE(Subscript, cst_index top; cst_index sub;, {
    n->top = top;
    n->sub = sub;
}, cst_index top, cst_index sub);
