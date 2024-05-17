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
        #define _cst__NODEPRINTSUB(type, pstr) do { fputs(prefix, stdout); for (int i = 0; i < strlen(#type); i++) putchar(' '); } while(0)
        #define _cst_NODEPRINTSUB(type, pstr, val) do { _cst__NODEPRINTSUB(type, pstr); printf("/" pstr "%s", cst_NODECAST(type, node)->val, suffix); } while(0)
        #define _cst_NODEPRINTSUBLIST(type, title, var, count, fmt) do { \
            _cst__NODEPRINTSUB(type, title); \
            printf("/" title "%s", suffix); \
            for (int i = 0; i < cst_NODECAST(type, node)->count; i++) { \
                _cst__NODEPRINTSUB(type, title); \
                printf("- " fmt "%s", cst_NODECAST(type, node)->var[i], suffix); \
            } } while(0)
        // Control
        case Entrypoint:
            _cst_NODEPRINTTOP(Entrypoint, "node[noderef]: %u", node);
            break;
        case ExtraData:
            _cst_NODEPRINTTOP(ExtraData, "meta[char*]: %s", meta);
            _cst_NODEPRINTSUB(ExtraData, "data[char*]: %s", data);
            break;
        case Block:
            _cst_NODEPRINTTOP(Block, "node_count[int]: %d", node_count);
            _cst_NODEPRINTSUBLIST(Block, "nodes[noderef*]:", nodes, node_count, "%u");
            break;
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
            break;
        // Accesses
        case Attribute:
            _cst_NODEPRINTTOP(Attribute, "top[noderef]: %u", top);
            _cst_NODEPRINTSUB(Attribute, "sub[noderef]: %u", sub);
            break;
        case Subscript:
            _cst_NODEPRINTTOP(Subscript, "top[noderef]: %u", top);
            _cst_NODEPRINTSUB(Subscript, "sub[noderef]: %u", sub);
            break;
        // Procedures
        case ProcInvoke:
            _cst_NODEPRINTTOP(ProcInvoke, "proc[noderef]: %u", proc);
            _cst_NODEPRINTSUB(ProcInvoke, "arglen[int]: %u", arglen);
            _cst_NODEPRINTSUBLIST(ProcInvoke, "args[noderef*]:", args, arglen, "%u");
            _cst_NODEPRINTSUB(ProcInvoke, "kwarglen[int]: %u", kwarglen);
            _cst__NODEPRINTSUB(ProcInvoke, "kwargs[ProcKwarg*]:");
            printf("/kwargs[ProcKwarg*]: %s", suffix);
            for (int i = 0; i < cst_NODECAST(ProcInvoke, node)->kwarglen; i++) {
                _cst__NODEPRINTSUB(ProcInvoke, "kwargs[ProcKwarg*]:");
                printf("- key[noderef]: %u%s", cst_NODECAST(ProcInvoke, node)->kwargs[i]->key, suffix);
                _cst__NODEPRINTSUB(ProcInvoke, "kwargs[ProcKwarg*]:");
                printf("  val[noderef]: %u%s", cst_NODECAST(ProcInvoke, node)->kwargs[i]->val, suffix);
            }
            break;
        case ProcExpr:
            _cst_NODEPRINTTOP(ProcExpr, "rtype[noderef]: %u", rtype);
            _cst_NODEPRINTSUB(ProcExpr, "param_len[int]: %u", param_len);
            _cst__NODEPRINTSUB(ProcExpr, "params[ProcParam*]:");
            printf("/params[ProcParam*]: %s", suffix);
            for (int i = 0; i < cst_NODECAST(ProcExpr, node)->param_len; i++) {
                _cst__NODEPRINTSUB(ProcExpr, "params[ProcParam*]:");
                printf("- type[noderef]: %u%s", cst_NODECAST(ProcExpr, node)->params[i].type, suffix);
                _cst__NODEPRINTSUB(ProcExpr, "params[ProcParam*]:");
                printf("  name[noderef]: %u%s", cst_NODECAST(ProcExpr, node)->params[i].name, suffix);
                _cst__NODEPRINTSUB(ProcExpr, "params[ProcParam*]:");
                printf("  val[noderef]: %u%s", cst_NODECAST(ProcExpr, node)->params[i].val, suffix);
                _cst__NODEPRINTSUB(ProcExpr, "params[ProcParam*]:");
                printf("  has_default[bool]: %d%s", cst_NODECAST(ProcExpr, node)->params[i].has_default, suffix);
                _cst__NODEPRINTSUB(ProcExpr, "params[ProcParam*]:");
                printf("  param_type: %d%s", cst_NODECAST(ProcExpr, node)->params[i].param_type, suffix);
            }
            break;
        case ProcStmt:
            _cst_NODEPRINTTOP(ProcStmt, "name[noderef]: %u", name);
            _cst_NODEPRINTSUB(ProcStmt, "rtype[noderef]: %u", rtype);
            _cst_NODEPRINTSUB(ProcStmt, "param_len[int]: %u", param_len);
            _cst__NODEPRINTSUB(ProcStmt, "params[ProcParam*]:");
            printf("/params[ProcParam*]: %s", suffix);
            for (int i = 0; i < cst_NODECAST(ProcStmt, node)->param_len; i++) {
                _cst__NODEPRINTSUB(ProcStmt, "params[ProcParam*]:");
                printf("- type[noderef]: %u%s", cst_NODECAST(ProcStmt, node)->params[i].type, suffix);
                _cst__NODEPRINTSUB(ProcStmt, "params[ProcParam*]:");
                printf("  name[noderef]: %u%s", cst_NODECAST(ProcStmt, node)->params[i].name, suffix);
                _cst__NODEPRINTSUB(ProcStmt, "params[ProcParam*]:");
                printf("  val[noderef]: %u%s", cst_NODECAST(ProcStmt, node)->params[i].val, suffix);
                _cst__NODEPRINTSUB(ProcStmt, "params[ProcParam*]:");
                printf("  has_default[bool]: %d%s", cst_NODECAST(ProcStmt, node)->params[i].has_default, suffix);
                _cst__NODEPRINTSUB(ProcStmt, "params[ProcParam*]:");
                printf("  param_type: %d%s", cst_NODECAST(ProcStmt, node)->params[i].param_type, suffix);
            }
            break;
        // Statements
        case IfStmt:
            _cst_NODEPRINTTOP(IfStmt, "condition[noderef]: %u", condition);
            _cst_NODEPRINTSUB(IfStmt, "body[noderef]: %u", body);
            _cst_NODEPRINTSUB(IfStmt, "next[noderef]: %u", next);
            break;
        case ElIfStmt:
            _cst_NODEPRINTTOP(ElIfStmt, "condition[noderef]: %u", condition);
            _cst_NODEPRINTSUB(ElIfStmt, "body[noderef]: %u", body);
            _cst_NODEPRINTSUB(ElIfStmt, "next[noderef]: %u", next);
            break;
        case ElseStmt:
            _cst_NODEPRINTTOP(ElseStmt, "body[noderef]: %u", body);
            break;
        case ForStmt:
            _cst_NODEPRINTTOP(ForStmt, "init[noderef]: %u", init);
            _cst_NODEPRINTSUB(ForStmt, "cond[noderef]: %u", cond);
            _cst_NODEPRINTSUB(ForStmt, "iter[noderef]: %u", iter);
            _cst_NODEPRINTSUB(ForStmt, "body[noderef]: %u", body);
            break;
        case WhileStmt:
            _cst_NODEPRINTTOP(WhileStmt, "cond[noderef]: %u", cond);
            _cst_NODEPRINTSUB(WhileStmt, "body[noderef]: %u", body);
            _cst_NODEPRINTSUB(WhileStmt, "do_while[bool]: %d", do_while);
            break;
        // Fail
        default: assert(false);
        #undef _cst_NODEPRINTTOP
        #undef _cst__NODEPRINTSUB
        #undef _cst_NODEPRINTSUB
        #undef _cst_NODEPRINTSUBLIST
    }
}
