cst_MKNODETYPE(IfStmt, cst_index condition; cst_index body; cst_index next;, {
    n->condition = condition;
    n->body = body;
    n->next = next;
}, cst_index condition, cst_index body, cst_index next);

cst_MKNODETYPE(ElIfStmt, cst_index condition; cst_index body; cst_index next;, {
    n->condition = condition;
    n->body = body;
    n->next = next;
}, cst_index condition, cst_index body, cst_index next);

cst_MKNODETYPE_S(ElseStmt, cst_index, body);
