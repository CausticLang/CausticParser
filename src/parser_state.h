#ifndef cap_PARSER_STATE_H_GUARD
#define cap_PARSER_STATE_H_GUARD 1

#include "ast/cst.h"

struct cap_ParserState {
    struct cst_Root* root;
    struct cap_ParserStack* stack;
};

struct cap_ParserStack {
    struct cap_ParserStack* prev;
    void* nodes;
    unsigned int nodecount;
    int mark;
};


struct cap_ParserStack* cap_ps_push(struct cap_ParserState* state);
void* cap_ps_pop(struct cap_ParserState* state);


#endif
