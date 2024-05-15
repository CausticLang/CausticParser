#ifndef cap_PARSER_STATE_H_GUARD
#define cap_PARSER_STATE_H_GUARD 1

#include "ast/cst.h"

struct cap_ParserState {
    struct cst_Root* root;
    struct cap_ParserStack* stack;
};

struct cap_ParserStack {
    struct cap_ParserStack* prev;
    cst_index* nodes;
    unsigned int nodecount;
};


struct cap_ParserStack* cap_ps_push(struct cap_ParserState* state);
cst_index* cap_ps_pop(struct cap_ParserState* state);


#endif
