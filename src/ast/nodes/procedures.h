struct cst_ProcKwarg {
    cst_index key; cst_index val;
};

cst_MKNODETYPE(ProcInvoke,
    cst_index proc;
    cst_index* args;
    size_t arglen;
    struct cst_ProcKwarg** kwargs;
    size_t kwarglen;,
    {
        n->proc = proc;
        n->args = args;
        n->arglen = arglen;
        n->kwargs = kwargs;
        n->kwarglen = kwarglen;
    }, cst_index proc, cst_index* args, size_t arglen, struct cst_ProcKwarg** kwargs, size_t kwarglen);

struct cst_ProcParam {
    cst_index type; cst_index name; cst_index val;
    bool has_default;
    enum:char {POS_ONLY, POS_OR_KW, KW_ONLY} param_type;
};

cst_MKNODETYPE(ProcExpr,
    cst_index rtype;
    struct cst_ProcParam* params; size_t param_len;
    cst_index body;,
    {
        n->rtype = rtype;
        n->params = params;
        n->param_len = param_len;
        n->body = body;
    }, cst_index rtype, struct cst_ProcParam* params, size_t param_len, cst_index body);

cst_MKNODETYPE(ProcStmt,
    cst_index name;
    cst_index rtype;
    struct cst_ProcParam* params; size_t param_len;
    cst_index body;,
    {
        n->name = name;
        n->rtype = rtype;
        n->params = params;
        n->param_len = param_len;
        n->body = body;
    }, cst_index name, cst_index rtype, struct cst_ProcParam* params, size_t param_len, cst_index body);

#define cst_PROC_ADD_PARAM(node, pt, t, n, hd, d) do { \
    (node)->params = realloc((node)->params, (++(node)->param_len)*sizeof(struct cst_ProcParam)); \
    (node)->params[(node)->param_len-1].type = t; \
    (node)->params[(node)->param_len-1].name = n; \
    (node)->params[(node)->param_len-1].val = d; \
    (node)->params[(node)->param_len-1].has_default = hd; \
    (node)->params[(node)->param_len-1].param_type = pt; \
} while(0)
