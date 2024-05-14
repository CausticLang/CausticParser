#include <stdbool.h>

cst_MKNODETYPE_S(Identifier, char*, val);

cst_MKNODETYPE_S(Bool, bool, val);

cst_MKNODETYPE(Integer, char* val; unsigned int base;, {
    n->val = val;
    n->base = base;
}, char* val, unsigned int base);

cst_MKNODETYPE(Float, char* integer; char* fractional; int exp;, {
    n->integer = integer;
    n->fractional = fractional;
    n->exp = exp;
}, char* integer, char* fractional, int exp);

cst_MKNODETYPE_S(Char, char*, val);

cst_MKNODETYPE(Bytes, char* val; bool raw;, {
    n->val = val;
    n->raw = raw;
}, char* val, bool raw);
cst_MKNODETYPE(String, char* val; bool raw;, {
    n->val = val;
    n->raw = raw;
}, char* val, bool raw);
