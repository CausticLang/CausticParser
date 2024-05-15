#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

void cst_print_node(struct cst_NodeBase* node, char* prefix, char* suffix) {
    if (node == NULL) {
        printf("%s<null node>%s", prefix, suffix);
        return;
    }
    if (node->is_freed) {
        printf("%s<freed node>%s", prefix, suffix);
        return;
    }
    printf("%stype: %d%s", prefix, node->type, suffix);
    printf("%sat pos: %u - %u%s", prefix, node->pos_start, node->pos_end, suffix);
    switch (node->type) {
        #define _cst_NODEPRINTTOP(type, pstr, val) printf("%s" #type "/" pstr "%s", prefix, cst_NODECAST(type, node)->val, suffix)
        #define _cst_NODEPRINTSUB(type, pstr, val) do { fputs(prefix, stdout); for (int i = 0; i < strlen(#type); i++) putchar(' '); printf("/" pstr "%s", cst_NODECAST(type, node)->val, suffix); } while(0); 
        // Atoms
        case Identifier:
            _cst_NODEPRINTTOP(Identifier, "val[str]: %s", val);
            break;
        case Bool:
            _cst_NODEPRINTTOP(Bool, "val[bool]: %u", val);
            break;
        case Integer:
            _cst_NODEPRINTTOP(Integer, "val[str]: %s", val);
            _cst_NODEPRINTSUB(Integer, "base[int]: %u", base);
            break;
        case Float:
            _cst_NODEPRINTTOP(Float, "integer[str]: %s", integer);
            _cst_NODEPRINTSUB(Float, "fractional[str]: %s", fractional);
            _cst_NODEPRINTSUB(Float, "exp[int]: %d", exp); 
            break;
        case Char:
            _cst_NODEPRINTTOP(Char, "val[str]: %s", val);
            break;
        case Bytes:
            _cst_NODEPRINTTOP(Bytes, "val[str]: %s", val);
            _cst_NODEPRINTSUB(Bytes, "raw[bool]: %d", raw);
            break;
        case String:
            _cst_NODEPRINTTOP(String, "val[str]: %s", val);
            _cst_NODEPRINTSUB(String, "raw[bool]: %d", raw);
            break;
        // Operators
        case UnaryOp:
            _cst_NODEPRINTTOP(UnaryOp, "op: %d", op);
            _cst_NODEPRINTSUB(UnaryOp, "target[noderef]: %u", target);
            break;
        case BinaryOp:
            _cst_NODEPRINTTOP(BinaryOp, "op: %d", op);
            _cst_NODEPRINTSUB(BinaryOp, "left[noderef]: %u", left);
            _cst_NODEPRINTSUB(BinaryOp, "right[noderef]: %u", right);
            break;
        case TernaryOp:
            _cst_NODEPRINTTOP(TernaryOp, "a[noderef]: %u", a);
            _cst_NODEPRINTSUB(TernaryOp, "b[noderef]: %u", b);
            _cst_NODEPRINTSUB(TernaryOp, "c[noderef]: %u", c);
        // Fail
        default: assert(false);
        #undef _cst_NODEPRINTTOP
        #undef _cst_NODEPRINTSUB
    }
}