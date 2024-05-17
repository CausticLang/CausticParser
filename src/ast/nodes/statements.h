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


cst_MKNODETYPE_E(PassStmt);

enum cst_FlowControlType:char {
    BREAK, CONTINUE,
};
cst_MKNODETYPE_S(FlowControlStmt, enum cst_FlowControlType, type);

cst_MKNODETYPE_S(ReturnStmt, cst_index, val);


cst_MKNODETYPE(Declaration, cst_index type; cst_index name; cst_index val;, {
    n->type = type;
    n->name = name;
    n->val = val;
}, cst_index type, cst_index name, cst_index val);
cst_MKNODETYPE(Assignment, cst_index target; cst_index val;, {
    n->target = target;
    n->val = val;
}, cst_index target, cst_index val);
