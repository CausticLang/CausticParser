struct cst_nBlock {
    struct cst_NodeBase _base;
    size_t node_count;
    cst_index* nodes;
};
struct cst_nBlock* cst_ninit_Block(struct cst_nBlock* n, unsigned int p_start, unsigned int p_end) {
    cst_NODEDOWNCAST(n)->type = Block;
    cst_NODEDOWNCAST(n)->is_freed = false;
    cst_NODEDOWNCAST(n)->pos_start = p_start;
    cst_NODEDOWNCAST(n)->pos_end = p_end;
    n->node_count = 0;
    n->nodes = NULL;
    return n;
};
void cst_nBlock_addnode(struct cst_nBlock* block, cst_index node) {
    block->nodes = realloc(block->nodes, (++block->node_count)*sizeof(cst_index));
    block->nodes[block->node_count-1] = node;
}
