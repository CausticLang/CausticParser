#define _cap_PCC0 _0
#define _cap_PCC0s _0s
#define _cap_PCC0e _0e

#define _cap_COPYSTR(frm) \
    strcpy(malloc(((strlen(frm) + 1) * sizeof(char))), frm)
#define _cap_ERROR(rname, msg) \
    cap_error(rname, msg, _cap_PCC0, _cap_PCC0s, _cap_PCC0e)

#define _cap_MKNODE(type) \
    (struct cst_n##type*)malloc(sizeof(struct cst_n##type))
#define _cap_MKINITNODE(type, ...) \
    cst_ninit_##type(_cap_MKNODE(type), _cap_PCC0s, _cap_PCC0e, __VA_ARGS__)

#define _cap_ADDNODE(node) \
    cst_node_add(auxil->root, cst_NODEDOWNCAST(node))
#define _cap_LASTNODE auxil->root->node_count-1
