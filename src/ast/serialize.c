#include "serialize.h"

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>


#define _cst_GENSERIALIZE(name, plural, ftype, ttype) \
    static inline void _cst_serialize_##name(ftype val, FILE* stream) { \
        if (cst_SERIALIZE_DEBUG) \
            fprintf(stderr, "serialize debug: write " #ftype " of size %d as " #ttype " of size %d to stream\n", sizeof(ftype), sizeof(ttype)); \
        fwrite((ttype*)&val, sizeof(ttype), 1, stream); \
    } \
    static inline void _cst_serialize_##plural(ftype* vals, size_t count, FILE* stream) { \
        if (cst_SERIALIZE_DEBUG) \
            fprintf(stderr, "serialize debug: write %d of " #ftype " of size %d as " #ttype " of size %d to stream\n", count, sizeof(ftype), sizeof(ttype)); \
        fwrite((ttype*)vals, sizeof(ttype), count, stream); \
    } \
    static inline ftype _cst_deserialize_##name(FILE* stream) { \
        if (cst_SERIALIZE_DEBUG) \
            fprintf(stderr, "serialize debug: read " #ftype " of size %d as " #ttype " of size %d from stream\n", sizeof(ftype), sizeof(ttype)); \
        ftype val; \
        fread((ftype*)&val, sizeof(ttype), 1, stream); \
        return (ttype)val; \
    } \
    static inline ftype* _cst_deserialize_##plural(size_t count, FILE* stream) { \
        if (cst_SERIALIZE_DEBUG) \
            fprintf(stderr, "serialize debug: read %d of " #ftype " of size %d as " #ttype " of size %d from stream\n", count, sizeof(ftype), sizeof(ttype)); \
        ttype* vals = malloc(count*sizeof(ttype)); \
        fread(vals, sizeof(ttype), count, stream); \
        return (ftype*)vals; \
    }

_cst_GENSERIALIZE(bool, bools, bool, bool);
_cst_GENSERIALIZE(int, ints, int, int16_t);
_cst_GENSERIALIZE(uint, uints, unsigned int, uint16_t);
_cst_GENSERIALIZE(sizet, sizets, size_t, uint16_t);
_cst_GENSERIALIZE(index, indexes, cst_index, uint16_t);
_cst_GENSERIALIZE(kwarg, kwargs, struct cst_ProcKwarg, struct cst_ProcKwarg);
_cst_GENSERIALIZE(param, params, struct cst_ProcParam, struct cst_ProcParam);
_cst_GENSERIALIZE(enum_member, enum_members, struct cst_nEnum_Member, struct cst_nEnum_Member);
_cst_GENSERIALIZE(struct_member, struct_members, struct cst_nStruct_Member, struct cst_nStruct_Member);
_cst_GENSERIALIZE(class_member, class_members, struct cst_nClass_Member, struct cst_nClass_Member);

#undef _cst_GENSERIALIZE

static inline void _cst_serialize_chars(char* val, FILE* stream) {
    fputs(val, stream);
    fputc('\0', stream);
}

void cst_nserialize_metadata_to(struct cst_NodeBase* node, FILE* stream) {
    assert(!node->is_freed);
    assert(node->type >= 0);
    assert(node->type < 128);
    fputc((char)node->type, stream);
    _cst_serialize_uint(node->pos_start, stream);
    _cst_serialize_uint(node->pos_end, stream);
    _cst_serialize_uint(node->lineno, stream);
    _cst_serialize_uint(node->colno, stream);
}
void cst_nserialize_to(struct cst_NodeBase* node, FILE* stream) {
    cst_nserialize_metadata_to(node, stream);
    switch (node->type) {
        // Control
        case Entrypoint:
            _cst_serialize_uint(cst_NODECAST(Entrypoint, node)->eof_pos, stream);
            _cst_serialize_index(cst_NODECAST(Entrypoint, node)->node, stream);
            break;
        case ExtraData:
            _cst_serialize_chars(cst_NODECAST(ExtraData, node)->meta, stream);
            _cst_serialize_chars(cst_NODECAST(ExtraData, node)->data, stream);
            break;
        case Block:
            _cst_serialize_sizet(cst_NODECAST(Block, node)->node_count, stream);
            _cst_serialize_indexes(cst_NODECAST(Block, node)->nodes, cst_NODECAST(Block, node)->node_count, stream);
            break;
        // Types
        case Type:
            _cst_serialize_index(cst_NODECAST(Type, node)->top, stream);
            _cst_serialize_index(cst_NODECAST(Type, node)->sub, stream);
            break;
        case Enum:
            _cst_serialize_index(cst_NODECAST(Enum, node)->name, stream);
            _cst_serialize_index(cst_NODECAST(Enum, node)->base, stream);
            _cst_serialize_sizet(cst_NODECAST(Enum, node)->member_count, stream);
            _cst_serialize_enum_members(cst_NODECAST(Enum, node)->members, cst_NODECAST(Enum, node)->member_count, stream);
            break;
        case Struct:
            _cst_serialize_index(cst_NODECAST(Struct, node)->name, stream);
            _cst_serialize_index(cst_NODECAST(Struct, node)->base, stream);
            _cst_serialize_sizet(cst_NODECAST(Struct, node)->member_count, stream);
            _cst_serialize_struct_members(cst_NODECAST(Struct, node)->members, cst_NODECAST(Struct, node)->member_count, stream);
            break;
        case StructEnum:
            _cst_serialize_index(cst_NODECAST(StructEnum, node)->name, stream);
            _cst_serialize_index(cst_NODECAST(StructEnum, node)->base, stream);
            _cst_serialize_sizet(cst_NODECAST(StructEnum, node)->member_count, stream);
            _cst_serialize_indexes(cst_NODECAST(StructEnum, node)->members, cst_NODECAST(StructEnum, node)->member_count, stream);
        case Class:
            _cst_serialize_index(cst_NODECAST(Class, node)->name, stream);
            _cst_serialize_index(cst_NODECAST(Class, node)->base, stream);
            _cst_serialize_sizet(cst_NODECAST(Class, node)->member_count, stream);
            for (int i = 0; i < cst_NODECAST(Class, node)->member_count; i++) {
                fputc((cst_NODECAST(Class, node)->members[i].is_static << 1) | cst_NODECAST(Class, node)->members[i].is_method, stream);
                if (cst_NODECAST(Class, node)->members[i].is_method)
                    _cst_serialize_index(cst_NODECAST(Class, node)->members[i].method, stream);
                else
                    _cst_serialize_struct_members((cst_NODECAST(Class, node)->members[i].member), 1, stream);
            }
            break;
        // Atoms
        case Identifier:
            _cst_serialize_chars(cst_NODECAST(Identifier, node)->val, stream);
            break;
        case Bool:
            _cst_serialize_bool(cst_NODECAST(Bool, node)->val, stream);
            break;
        case Integer:
            _cst_serialize_chars(cst_NODECAST(Integer, node)->val, stream);
            _cst_serialize_uint(cst_NODECAST(Integer, node)->base, stream);
            break;
        case Float:
            _cst_serialize_chars(cst_NODECAST(Float, node)->integer, stream);
            _cst_serialize_chars(cst_NODECAST(Float, node)->fractional, stream);
            _cst_serialize_int(cst_NODECAST(Float, node)->exp, stream);
            break;
        case Char:
            _cst_serialize_chars(cst_NODECAST(Char, node)->val, stream);
            break;
        case Bytes:
            _cst_serialize_chars(cst_NODECAST(Bytes, node)->val, stream);
            _cst_serialize_bool(cst_NODECAST(Bytes, node)->raw, stream);
            break;
        case String:
            _cst_serialize_chars(cst_NODECAST(String, node)->val, stream);
            _cst_serialize_bool(cst_NODECAST(String, node)->raw, stream);
            break;
        // Operations
        case UnaryOp:
            assert(cst_NODECAST(UnaryOp, node)->op >= 0);
            assert(cst_NODECAST(UnaryOp, node)->op < 128);
            fputc((char)(cst_NODECAST(UnaryOp, node)->op), stream);
            _cst_serialize_index(cst_NODECAST(UnaryOp, node)->target, stream);
            break;
        case BinaryOp:
            assert(cst_NODECAST(BinaryOp, node)->op >= 0);
            assert(cst_NODECAST(BinaryOp, node)->op < 128);
            fputc((char)(cst_NODECAST(BinaryOp, node)->op), stream);
            _cst_serialize_index(cst_NODECAST(BinaryOp, node)->left, stream);
            _cst_serialize_index(cst_NODECAST(BinaryOp, node)->right, stream);
            break;
        case TernaryOp:
            assert(cst_NODECAST(TernaryOp, node)->op >= 0);
            assert(cst_NODECAST(TernaryOp, node)->op < 128);
            fputc((char)(cst_NODECAST(TernaryOp, node)->op), stream);
            _cst_serialize_index(cst_NODECAST(TernaryOp, node)->a, stream);
            _cst_serialize_index(cst_NODECAST(TernaryOp, node)->b, stream);
            _cst_serialize_index(cst_NODECAST(TernaryOp, node)->c, stream);
            break;
        // Accesses
        case Attribute:
            _cst_serialize_index(cst_NODECAST(Attribute, node)->top, stream);
            _cst_serialize_index(cst_NODECAST(Attribute, node)->sub, stream);
            break;
        case Subscript:
            _cst_serialize_index(cst_NODECAST(Subscript, node)->top, stream);
            _cst_serialize_index(cst_NODECAST(Subscript, node)->sub, stream);
            break;
        // Procedures
        case ProcInvoke:
            _cst_serialize_index(cst_NODECAST(ProcInvoke, node)->proc, stream);
            _cst_serialize_sizet(cst_NODECAST(ProcInvoke, node)->arglen, stream);
            _cst_serialize_indexes(cst_NODECAST(ProcInvoke, node)->args, cst_NODECAST(ProcInvoke, node)->arglen, stream);
            _cst_serialize_sizet(cst_NODECAST(ProcInvoke, node)->kwarglen, stream);
            for (int i = 0; i < cst_NODECAST(ProcInvoke, node)->kwarglen; i++) {
                _cst_serialize_index(cst_NODECAST(ProcInvoke, node)->kwargs[i]->key, stream);
                _cst_serialize_index(cst_NODECAST(ProcInvoke, node)->kwargs[i]->val, stream);
            }
            break;
        case ProcExpr:
            _cst_serialize_index(cst_NODECAST(ProcExpr, node)->rtype, stream);
            _cst_serialize_sizet(cst_NODECAST(ProcExpr, node)->param_len, stream);
            _cst_serialize_params(cst_NODECAST(ProcExpr, node)->params, cst_NODECAST(ProcExpr, node)->param_len, stream);
            _cst_serialize_index(cst_NODECAST(ProcExpr, node)->body, stream);
            break;
        case ProcStmt:
            _cst_serialize_index(cst_NODECAST(ProcStmt, node)->name, stream);
            _cst_serialize_index(cst_NODECAST(ProcStmt, node)->rtype, stream);
            _cst_serialize_sizet(cst_NODECAST(ProcStmt, node)->param_len, stream);
            _cst_serialize_params(cst_NODECAST(ProcStmt, node)->params, cst_NODECAST(ProcStmt, node)->param_len, stream);
            _cst_serialize_index(cst_NODECAST(ProcStmt, node)->body, stream);
            break;
        // Statements
        case IfStmt:
            _cst_serialize_index(cst_NODECAST(IfStmt, node)->condition, stream);
            _cst_serialize_index(cst_NODECAST(IfStmt, node)->body, stream);
            _cst_serialize_index(cst_NODECAST(IfStmt, node)->next, stream);
            break;
        case ElIfStmt:
            _cst_serialize_index(cst_NODECAST(ElIfStmt, node)->condition, stream);
            _cst_serialize_index(cst_NODECAST(ElIfStmt, node)->body, stream);
            _cst_serialize_index(cst_NODECAST(ElIfStmt, node)->next, stream);
            break;
        case ElseStmt:
            _cst_serialize_index(cst_NODECAST(ElseStmt, node)->body, stream);
            break;
        case ForStmt:
            _cst_serialize_index(cst_NODECAST(ForStmt, node)->init, stream);
            _cst_serialize_index(cst_NODECAST(ForStmt, node)->cond, stream);
            _cst_serialize_index(cst_NODECAST(ForStmt, node)->iter, stream);
            _cst_serialize_index(cst_NODECAST(ForStmt, node)->body, stream);
            break;
        case WhileStmt:
            _cst_serialize_index(cst_NODECAST(WhileStmt, node)->cond, stream);
            _cst_serialize_index(cst_NODECAST(WhileStmt, node)->body, stream);
            _cst_serialize_bool(cst_NODECAST(WhileStmt, node)->do_while, stream);
            break;
        case PassStmt:
            break;
        case FlowControlStmt:
            assert(cst_NODECAST(FlowControlStmt, node)->type >= 0);
            assert(cst_NODECAST(FlowControlStmt, node)->type < 128);
            fputc((char)(cst_NODECAST(FlowControlStmt, node)->type), stream);
            break;
        case ReturnStmt:
            _cst_serialize_index(cst_NODECAST(ReturnStmt, node)->val, stream);
            break;
        case Declaration:
            _cst_serialize_index(cst_NODECAST(Declaration, node)->type, stream);
            _cst_serialize_index(cst_NODECAST(Declaration, node)->name, stream);
            _cst_serialize_index(cst_NODECAST(Declaration, node)->val, stream);
            break;
        case Assignment:
            _cst_serialize_index(cst_NODECAST(Assignment, node)->target, stream);
            _cst_serialize_index(cst_NODECAST(Assignment, node)->val, stream);
            break;
        // Default
        default: assert(false);
    }
}

void cst_serialize_to(struct cst_Root* root, FILE* stream) {
    for (int i = 0; i < root->node_count; i++) {
        fputc('\1', stream);
        cst_nserialize_to(root->nodes[i], stream);
    }
}



static inline char* _cst_deserialize_chars(FILE* stream) {
    int len = 0;
    int size = cst_DESERIALIZE_CHAR_BUFF_SIZE;
    char* val = malloc(sizeof(char)*size);
    for (char c; (c = fgetc(stream)) != '\0'; len++) {
        assert(!feof(stream));
        val[len] = c;
        if (len > size) {
            size += cst_DESERIALIZE_CHAR_BUFF_SIZE;
            val = realloc(val, sizeof(char)*size);
        }
    }
    return realloc(val, sizeof(char)*size);
}

struct cst_NodeBase* cst_ndeserialize_from(FILE* stream) {
    assert(!feof(stream));
    enum cst_NodeType type = (enum cst_NodeType)fgetc(stream);
    struct cst_NodeBase* node;
    // Metadata
    unsigned int pos_start = _cst_deserialize_uint(stream);
    unsigned int pos_end = _cst_deserialize_uint(stream);
    unsigned int lineno = _cst_deserialize_uint(stream);
    unsigned int colno = _cst_deserialize_uint(stream);
    // Type
    #if cst_SERIALIZE_DEBUG
        fprintf(stderr, "serialize debug: deserializing type %u\n", type);
    #endif
    switch (type) {
        #define _cst_ALLOCNODE(type) node = cst_NODEDOWNCAST(malloc(sizeof(struct cst_n##type)))
        // Control
        case Entrypoint:
            _cst_ALLOCNODE(Entrypoint);
            cst_NODECAST(Entrypoint, node)->eof_pos = _cst_deserialize_uint(stream);
            cst_NODECAST(Entrypoint, node)->node = _cst_deserialize_index(stream);
            break;
        case ExtraData:
            _cst_ALLOCNODE(ExtraData);
            cst_NODECAST(ExtraData, node)->meta = _cst_deserialize_chars(stream);
            cst_NODECAST(ExtraData, node)->data = _cst_deserialize_chars(stream);
            break;
        case Block:
            _cst_ALLOCNODE(Block);
            size_t ncount = cst_NODECAST(Block, node)->node_count = _cst_deserialize_sizet(stream);
            cst_NODECAST(Block, node)->nodes = _cst_deserialize_indexes(ncount, stream);
            break;
        // Types
        case Type:
            _cst_ALLOCNODE(Type);
            cst_NODECAST(Type, node)->top = _cst_deserialize_index(stream);
            cst_NODECAST(Type, node)->sub = _cst_deserialize_index(stream);
            break;
        case Enum:
            _cst_ALLOCNODE(Enum);
            cst_NODECAST(Enum, node)->name = _cst_deserialize_index(stream);
            cst_NODECAST(Enum, node)->base = _cst_deserialize_index(stream);
            size_t member_count = cst_NODECAST(Enum, node)->member_count = _cst_deserialize_sizet(stream);
            cst_NODECAST(Enum, node)->members = _cst_deserialize_enum_members(member_count, stream);
            break;
        case Struct:
            _cst_ALLOCNODE(Struct);
            cst_NODECAST(Struct, node)->name = _cst_deserialize_index(stream);
            cst_NODECAST(Struct, node)->base = _cst_deserialize_index(stream);
            /*size_t (decl in `Enum:`)*/ member_count = cst_NODECAST(Struct, node)->member_count = _cst_deserialize_sizet(stream);
            cst_NODECAST(Struct, node)->members = _cst_deserialize_struct_members(member_count, stream);
            break;
        case StructEnum:
            _cst_ALLOCNODE(StructEnum);
            cst_NODECAST(StructEnum, node)->name = _cst_deserialize_index(stream);
            cst_NODECAST(StructEnum, node)->base = _cst_deserialize_index(stream);
            /*size_t (decl in `Enum:`)*/ member_count = cst_NODECAST(StructEnum, node)->member_count = _cst_deserialize_sizet(stream);
            cst_NODECAST(StructEnum, node)->members = _cst_deserialize_indexes(member_count, stream);
            break;
        case Class:
            _cst_ALLOCNODE(Class);
            cst_NODECAST(Class, node)->name = _cst_deserialize_index(stream);
            cst_NODECAST(Class, node)->base = _cst_deserialize_index(stream);
            /*size_t (decl in `Enum:`)*/ member_count = cst_NODECAST(Class, node)->member_count = _cst_deserialize_sizet(stream);
            for (int i = 0; i < member_count; i++) {
                char props = fgetc(stream);
                cst_NODECAST(Class, node)->members[i].is_static = (bool)(props & 0b10);
                cst_NODECAST(Class, node)->members[i].is_method = (bool)(props & 0b01);
                if (props & 0b01)
                    cst_NODECAST(Class, node)->members[i].method = _cst_deserialize_index(stream);
                else
                    cst_NODECAST(Class, node)->members[i].member = _cst_deserialize_struct_members(1, stream);
            }
            break;
        // Atoms
        case Identifier:
            _cst_ALLOCNODE(Identifier);
            cst_NODECAST(Identifier, node)->val = _cst_deserialize_chars(stream);
            break;
        case Bool:
            _cst_ALLOCNODE(Bool);
            cst_NODECAST(Bool, node)->val = (bool)fgetc(stream);
            break;
        case Integer:
            _cst_ALLOCNODE(Integer);
            cst_NODECAST(Integer, node)->val = _cst_deserialize_chars(stream);
            cst_NODECAST(Integer, node)->base = _cst_deserialize_uint(stream);
            break;
        case Float:
            _cst_ALLOCNODE(Float);
            cst_NODECAST(Float, node)->integer = _cst_deserialize_chars(stream);
            cst_NODECAST(Float, node)->fractional = _cst_deserialize_chars(stream);
            cst_NODECAST(Float, node)->exp = _cst_deserialize_int(stream);
            break;
        case Char:
            _cst_ALLOCNODE(Char);
            cst_NODECAST(Char, node)->val = _cst_deserialize_chars(stream);
            break;
        case Bytes:
            _cst_ALLOCNODE(Bytes);
            cst_NODECAST(Bytes, node)->val = _cst_deserialize_chars(stream);
            cst_NODECAST(Bytes, node)->raw = (bool)fgetc(stream);
            break;
        case String:
            _cst_ALLOCNODE(String);
            cst_NODECAST(String, node)->val = _cst_deserialize_chars(stream);
            cst_NODECAST(String, node)->raw = (bool)fgetc(stream);
            break;
        // Operations
        case UnaryOp:
            _cst_ALLOCNODE(UnaryOp);
            cst_NODECAST(UnaryOp, node)->op = (enum cst_OperatorType)fgetc(stream);
            cst_NODECAST(UnaryOp, node)->target = _cst_deserialize_index(stream);
            break;
        case BinaryOp:
            _cst_ALLOCNODE(BinaryOp);
            cst_NODECAST(BinaryOp, node)->op = (enum cst_OperatorType)fgetc(stream);
            cst_NODECAST(BinaryOp, node)->left = _cst_deserialize_index(stream);
            cst_NODECAST(BinaryOp, node)->right = _cst_deserialize_index(stream);
            break;
        case TernaryOp:
            _cst_ALLOCNODE(TernaryOp);
            cst_NODECAST(TernaryOp, node)->op = (enum cst_OperatorType)fgetc(stream);
            cst_NODECAST(TernaryOp, node)->a = _cst_deserialize_index(stream);
            cst_NODECAST(TernaryOp, node)->b = _cst_deserialize_index(stream);
            cst_NODECAST(TernaryOp, node)->c = _cst_deserialize_index(stream);
            break;
        // Accesses
        case Attribute:
            _cst_ALLOCNODE(Attribute);
            cst_NODECAST(Attribute, node)->top = _cst_deserialize_index(stream);
            cst_NODECAST(Attribute, node)->sub = _cst_deserialize_index(stream);
            break;
        case Subscript:
            _cst_ALLOCNODE(Subscript);
            cst_NODECAST(Subscript, node)->top = _cst_deserialize_index(stream);
            cst_NODECAST(Subscript, node)->sub = _cst_deserialize_index(stream);
            break;
        // Procedures
        case ProcInvoke:
            _cst_ALLOCNODE(ProcInvoke);
            cst_NODECAST(ProcInvoke, node)->proc = _cst_deserialize_index(stream);
            size_t arglen = cst_NODECAST(ProcInvoke, node)->arglen = _cst_deserialize_sizet(stream);
            cst_NODECAST(ProcInvoke, node)->args = _cst_deserialize_indexes(arglen, stream);
            size_t kwarglen = cst_NODECAST(ProcInvoke, node)->kwarglen = _cst_deserialize_sizet(stream);
            cst_NODECAST(ProcInvoke, node)->kwargs = malloc(sizeof(struct cst_ProcKwarg*) * kwarglen);
            for (int i = 0; i < cst_NODECAST(ProcInvoke, node)->kwarglen; i++) {
                cst_NODECAST(ProcInvoke, node)->kwargs[i] = malloc(sizeof(struct cst_ProcKwarg));
                cst_NODECAST(ProcInvoke, node)->kwargs[i]->key = _cst_deserialize_index(stream);
                cst_NODECAST(ProcInvoke, node)->kwargs[i]->val = _cst_deserialize_index(stream);
            }
            break;
        case ProcExpr:
            _cst_ALLOCNODE(ProcExpr);
            cst_NODECAST(ProcExpr, node)->rtype = _cst_deserialize_index(stream);
            size_t param_len = cst_NODECAST(ProcExpr, node)->param_len = _cst_deserialize_sizet(stream);
            cst_NODECAST(ProcExpr, node)->params = _cst_deserialize_params(param_len, stream);
            cst_NODECAST(ProcExpr, node)->body = _cst_deserialize_index(stream);
            break;
        case ProcStmt:
            _cst_ALLOCNODE(ProcStmt);
            cst_NODECAST(ProcStmt, node)->name = _cst_deserialize_index(stream);
            cst_NODECAST(ProcStmt, node)->rtype = _cst_deserialize_index(stream);
            /*size_t (decl in `ProcExpr:`)*/ param_len = cst_NODECAST(ProcStmt, node)->param_len = _cst_deserialize_sizet(stream);
            cst_NODECAST(ProcStmt, node)->params = _cst_deserialize_params(param_len, stream);
            cst_NODECAST(ProcStmt, node)->body = _cst_deserialize_index(stream);
            break;
        // Statements
        case IfStmt:
            _cst_ALLOCNODE(IfStmt);
            cst_NODECAST(IfStmt, node)->condition = _cst_deserialize_index(stream);
            cst_NODECAST(IfStmt, node)->body = _cst_deserialize_index(stream);
            cst_NODECAST(IfStmt, node)->next = _cst_deserialize_index(stream);
            break;
        case ElIfStmt:
            _cst_ALLOCNODE(ElIfStmt);
            cst_NODECAST(ElIfStmt, node)->condition = _cst_deserialize_index(stream);
            cst_NODECAST(ElIfStmt, node)->body = _cst_deserialize_index(stream);
            cst_NODECAST(ElIfStmt, node)->next = _cst_deserialize_index(stream);
            break;
        case ElseStmt:
            _cst_ALLOCNODE(ElseStmt);
            cst_NODECAST(ElseStmt, node)->body = _cst_deserialize_index(stream);
            break;
        case ForStmt:
            _cst_ALLOCNODE(ForStmt);
            cst_NODECAST(ForStmt, node)->init = _cst_deserialize_index(stream);
            cst_NODECAST(ForStmt, node)->cond = _cst_deserialize_index(stream);
            cst_NODECAST(ForStmt, node)->iter = _cst_deserialize_index(stream);
            cst_NODECAST(ForStmt, node)->body = _cst_deserialize_index(stream);
            break;
        case WhileStmt:
            _cst_ALLOCNODE(WhileStmt);
            cst_NODECAST(WhileStmt, node)->cond = _cst_deserialize_index(stream);
            cst_NODECAST(WhileStmt, node)->body = _cst_deserialize_index(stream);
            cst_NODECAST(WhileStmt, node)->do_while = _cst_deserialize_bool(stream);
            break;
        case PassStmt:
            _cst_ALLOCNODE(PassStmt);
            break;
        case FlowControlStmt:
            _cst_ALLOCNODE(FlowControlStmt);
            cst_NODECAST(FlowControlStmt, node)->type = (enum cst_FlowControlType)fgetc(stream);
            break;
        case ReturnStmt:
            _cst_ALLOCNODE(ReturnStmt);
            cst_NODECAST(ReturnStmt, node)->val = _cst_deserialize_index(stream);
            break;
        case Declaration:
            _cst_ALLOCNODE(Declaration);
            cst_NODECAST(Declaration, node)->type = _cst_deserialize_index(stream);
            cst_NODECAST(Declaration, node)->name = _cst_deserialize_index(stream);
            cst_NODECAST(Declaration, node)->val = _cst_deserialize_index(stream);
            break;
        case Assignment:
            _cst_ALLOCNODE(Assignment);
            cst_NODECAST(Assignment, node)->target = _cst_deserialize_index(stream);
            cst_NODECAST(Assignment, node)->val = _cst_deserialize_index(stream);
            break;
        // Default
        default: assert(false);
        #undef _cst_ALLOCNODE
    }
    // Finish
    node->type = type;
    node->pos_start = pos_start;
    node->pos_end = pos_end;
    node->lineno = lineno;
    node->colno = colno;
    node->is_freed = false;
    return node;
}

void cst_deserialize_to(struct cst_Root* root, FILE* stream) {
    char c;
    while ((c = fgetc(stream)) != EOF) {
        assert(c == '\1');
        cst_node_add(root, cst_ndeserialize_from(stream));
    }
}
