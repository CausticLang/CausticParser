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


cst_MKNODETYPE(ForStmt, cst_index init; cst_index cond; cst_index iter; cst_index body;, {
    n->init = init;
    n->cond = cond;
    n->iter = iter;
    n->body = body;
}, cst_index init, cst_index cond, cst_index iter, cst_index body);

cst_MKNODETYPE(WhileStmt, cst_index cond; cst_index body; bool do_while;, {
    n->cond = cond;
    n->body = body;
    n->do_while = do_while;
}, cst_index cond, cst_index body, bool do_while);
