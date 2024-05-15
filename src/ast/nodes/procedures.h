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
    enum {POS_ONLY, POS_OR_KW, KW_ONLY} param_type;
};

cst_MKNODETYPE(ProcExpr,
    cst_index rtype;
    struct cst_ProcParam* params; size_t param_len;,
    {
        n->rtype = rtype;
        n->params = params;
        n->param_len = param_len;
    }, cst_index rtype, struct cst_ProcParam* params, size_t param_len);

cst_MKNODETYPE(ProcStmt,
    cst_index name;
    cst_index rtype;
    struct cst_ProcParam* params; size_t param_len;,
    {
        n->name = name;
        n->rtype = rtype;
        n->params = params;
        n->param_len = param_len;
    }, cst_index name, cst_index rtype, struct cst_ProcParam* params, size_t param_len);
