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
    printf("%sline: %u, col: %u%s", prefix, node->lineno, node->colno, suffix);
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
            _cst_NODEPRINTTOP(Entrypoint, "eof_pos[int]: %u", eof_pos);
            _cst_NODEPRINTSUB(Entrypoint, "node[noderef]: %u", node);
            break;
        case ExtraData:
            _cst_NODEPRINTTOP(ExtraData, "meta[char*]: %s", meta);
            _cst_NODEPRINTSUB(ExtraData, "data[char*]: %s", data);
            break;
        case Block:
            _cst_NODEPRINTTOP(Block, "node_count[int]: %d", node_count);
            _cst_NODEPRINTSUBLIST(Block, "nodes[noderef*]:", nodes, node_count, "%u");
            break;
        // Types
        case Type:
            _cst_NODEPRINTTOP(Type, "top[noderef]: %u", top);
            _cst_NODEPRINTSUB(Type, "sub[noderef]: %u", sub);
            break;
        case Enum:
            _cst_NODEPRINTTOP(Enum, "name[noderef]: %u", name);
            _cst_NODEPRINTSUB(Enum, "base[noderef]: %u", base);
            _cst_NODEPRINTSUB(Enum, "member_count[int]: %u", member_count);
            _cst__NODEPRINTSUB(Enum, "members[nEnum_Member*]:");
            printf("/members[nEnum_Member*]:%s", suffix);
            for (int i = 0; i < cst_NODECAST(Enum, node)->member_count; i++) {
                _cst__NODEPRINTSUB(Enum, "members[nEnum_Member*]:");
                printf("- name[noderef]: %u%s", cst_NODECAST(Enum, node)->members[i]->name, suffix);
                _cst__NODEPRINTSUB(Enum, "members[nEnum_Member*]:");
                printf("  val[noderef]: %u%s", cst_NODECAST(Enum, node)->members[i]->val, suffix);
            }
            break;
        case Struct:
            _cst_NODEPRINTTOP(Struct, "name[noderef]: %u", name);
            _cst_NODEPRINTSUB(Struct, "base[noderef]: %u", base);
            _cst_NODEPRINTSUB(Struct, "member_count[int]: %u", member_count);
            _cst__NODEPRINTSUB(Struct, "members[nStruct_Member*]:");
            printf("/members[nStruct_Member*]:%s", suffix);
            for (int i = 0; i < cst_NODECAST(Struct, node)->member_count; i++) {
                _cst__NODEPRINTSUB(Struct, "members[nStruct_Member*]:");
                printf("- type[noderef]: %u%s", cst_NODECAST(Struct, node)->members[i]->type, suffix);
                _cst__NODEPRINTSUB(Struct, "members[nStruct_Member*]:");
                printf("  name[noderef]: %u%s", cst_NODECAST(Struct, node)->members[i]->name, suffix);
                _cst__NODEPRINTSUB(Struct, "members[nStruct_Member*]:");
                printf("  val[noderef]: %u%s", cst_NODECAST(Struct, node)->members[i]->val, suffix);
            }
            break;
        case StructEnum:
            _cst_NODEPRINTTOP(StructEnum, "name[noderef]: %u", name);
            _cst_NODEPRINTSUB(StructEnum, "base[noderef]: %u", base);
            _cst_NODEPRINTSUB(StructEnum, "member_count[int]: %u", member_count);
            _cst_NODEPRINTSUBLIST(StructEnum, "members[noderef*]:", members, member_count, "%u");
            break;
        case Class:
            _cst_NODEPRINTTOP(Class, "name[noderef]: %u", name);
            _cst_NODEPRINTSUB(Class, "base[noderef]: %u", base);
            _cst_NODEPRINTSUB(Class, "member_count[int]: %u", member_count);
            _cst__NODEPRINTSUB(Class, "members[nClass_Member*]:");
            printf("/members[nClass_Member*]:%s", suffix);
            for (int i = 0; i < cst_NODECAST(Class, node)->member_count; i++) {
                _cst__NODEPRINTSUB(Class, "members[nClass_Member*]:");
                printf("- is_method[bool]: %d%s", cst_NODECAST(Class, node)->members[i]->is_method);
                _cst__NODEPRINTSUB(Class, "members[nClass_Member*]:");
                printf("  is_static[bool]: %d%s", cst_NODECAST(Class, node)->members[i]->is_static);
                _cst__NODEPRINTSUB(Class, "members[nClass_Member*]:");
                if (cst_NODECAST(Class, node)->is_method) {
                    printf("  method[noderef]: %u%s", cst_NODECAST(Class, node)->members[i]->member, suffix);
                    continue;
                }
                printf("  member/type[noderef]: %u%s", cst_NODECAST(Class, node)->members[i]->member->type, suffix);
                _cst__NODEPRINTSUB(Class, "members[nClass_Member*]:");
                printf("  member/name[noderef]: %u%s", cst_NODECAST(Class, node)->members[i]->member->name, suffix);
                _cst__NODEPRINTSUB(Class, "members[nClass_Member*]:");
                printf("  member/val[noderef]: %u%s", cst_NODECAST(Class, node)->members[i]->member->val, suffix);
            }
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
            _cst_NODEPRINTSUB(ProcExpr, "body[noderef]: %u", body);
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
            _cst_NODEPRINTSUB(ProcStmt, "body[noderef]: %u", body);
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
        case PassStmt:
            printf("%sPassStmt <no contents>%s", prefix, suffix);
            break;
        case FlowControlStmt:
            _cst_NODEPRINTTOP(FlowControlStmt, "type: %d", type);
            break;
        case Declaration:
            _cst_NODEPRINTTOP(Declaration, "type[noderef]: %u", type);
            _cst_NODEPRINTSUB(Declaration, "name[noderef]: %u", name);
            _cst_NODEPRINTSUB(Declaration, "val[noderef]: %u", val);
            break;
        case Assignment:
            _cst_NODEPRINTTOP(Assignment, "target[noderef]: %u", target);
            _cst_NODEPRINTSUB(Assignment, "val[noderef]: %u", val);
            break;
        // Fail
        default: assert(false);
        #undef _cst_NODEPRINTTOP
        #undef _cst__NODEPRINTSUB
        #undef _cst_NODEPRINTSUB
        #undef _cst_NODEPRINTSUBLIST
    }
}
