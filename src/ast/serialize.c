#include "serialize.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>


#define _cst_genserialize(name, plural, type) \
    static inline void _cst_serialize_##name(type val, FILE* stream) { \
        if (cst_SERIALIZE_DEBUG) \
            fprintf(stderr, "serialize debug: write " #type " of size %d to stream\n", sizeof(type)); \
        fwrite(&val, sizeof(type), 1, stream); \
    } \
    static inline void _cst_serialize_##plural(type* vals, size_t count, FILE* stream) { \
        if (cst_SERIALIZE_DEBUG) \
            fprintf(stderr, "serialize debug: write %d of " #type " of size %d to stream\n", count, sizeof(type)); \
        fwrite(vals, sizeof(type), count, stream); \
    } \
    static inline type _cst_deserialize_##name(FILE* stream) { \
        if (cst_SERIALIZE_DEBUG) \
            fprintf(stderr, "serialize debug: read " #type " of size %d from stream\n", sizeof(type)); \
        type val; \
        fread(&val, sizeof(type), 1, stream); \
        return val; \
    } \
    static inline type* _cst_deserialize_##plural(size_t count, FILE* stream) { \
        if (cst_SERIALIZE_DEBUG) \
            fprintf(stderr, "serialize debug: read %d of " #type " of size %d from stream\n", count, sizeof(type)); \
        type* vals = malloc(count*sizeof(type)); \
        fread(vals, sizeof(type), count, stream); \
        return vals; \
    }

_cst_genserialize(bool, bools, bool);
_cst_genserialize(int, ints, int);
_cst_genserialize(uint, uints, unsigned int);
_cst_genserialize(sizet, sizets, size_t);
_cst_genserialize(index, indexes, cst_index);
_cst_genserialize(kwarg, kwargs, struct cst_ProcKwarg);

static inline void _cst_serialize_chars(char* val, FILE* stream) {
    fputs(val, stream);
    fputc('\0', stream);
}

void cst_nserialize_metadata_to(struct cst_NodeBase* node, FILE* stream) {
    assert(!node->is_freed);
    assert(node->type < 256);
    fputc((char)node->type, stream);
    _cst_serialize_uint(node->pos_start, stream);
    _cst_serialize_uint(node->pos_end, stream);
}
void cst_nserialize_to(struct cst_NodeBase* node, FILE* stream) {
    cst_nserialize_metadata_to(node, stream);
    switch (node->type) {
        // Control
        case Entrypoint:
            _cst_serialize_index(cst_NODECAST(Entrypoint, node)->node, stream);
            break;
        case Block:
            _cst_serialize_int(cst_NODECAST(Block, node)->node_count, stream);
            _cst_serialize_indexes(cst_NODECAST(Block, node)->nodes, cst_NODECAST(Block, node)->node_count, stream);
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
            assert(cst_NODECAST(UnaryOp, node)->op < 256);
            fputc((char)(cst_NODECAST(UnaryOp, node)->op), stream);
            _cst_serialize_index(cst_NODECAST(UnaryOp, node)->target, stream);
            break;
        case BinaryOp:
            assert(cst_NODECAST(BinaryOp, node)->op < 256);
            fputc((char)(cst_NODECAST(BinaryOp, node)->op), stream);
            _cst_serialize_index(cst_NODECAST(BinaryOp, node)->left, stream);
            _cst_serialize_index(cst_NODECAST(BinaryOp, node)->right, stream);
            break;
        case TernaryOp:
            assert(cst_NODECAST(TernaryOp, node)->op < 256);
            fputc((char)(cst_NODECAST(TernaryOp, node)->op), stream);
            _cst_serialize_index(cst_NODECAST(TernaryOp, node)->a, stream);
            _cst_serialize_index(cst_NODECAST(TernaryOp, node)->b, stream);
            _cst_serialize_index(cst_NODECAST(TernaryOp, node)->c, stream);
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
    // Type
    switch (type) {
        #define _cst_ALLOCNODE(type) node = cst_NODEDOWNCAST(malloc(sizeof(struct cst_n##type)))
        // Control
        case Entrypoint:
            _cst_ALLOCNODE(Entrypoint);
            cst_NODECAST(Entrypoint, node)->node = _cst_deserialize_index(stream);
            break;
        case Block:
            _cst_ALLOCNODE(Block);
            size_t ncount = cst_NODECAST(Block, node)->node_count = _cst_deserialize_int(stream);
            cst_NODECAST(Block, node)->nodes = _cst_deserialize_indexes(ncount, stream);
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
            cst_NODECAST(UnaryOp, node)->op = (enum cst_NodeType)fgetc(stream);
            cst_NODECAST(UnaryOp, node)->target = _cst_deserialize_index(stream);
            break;
        case BinaryOp:
            _cst_ALLOCNODE(BinaryOp);
            cst_NODECAST(BinaryOp, node)->op = (enum cst_NodeType)fgetc(stream);
            cst_NODECAST(BinaryOp, node)->left = _cst_deserialize_index(stream);
            cst_NODECAST(BinaryOp, node)->right = _cst_deserialize_index(stream);
            break;
        case TernaryOp:
            _cst_ALLOCNODE(TernaryOp);
            cst_NODECAST(TernaryOp, node)->op = (enum cst_NodeType)fgetc(stream);
            cst_NODECAST(TernaryOp, node)->a = _cst_deserialize_index(stream);
            cst_NODECAST(TernaryOp, node)->b = _cst_deserialize_index(stream);
            cst_NODECAST(TernaryOp, node)->c = _cst_deserialize_index(stream);
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
        // Default
        default: assert(false);
        #undef _cst_ALLOCNODE
    }
    // Finish
    node->type = type;
    node->pos_start = pos_start;
    node->pos_end = pos_end;
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
