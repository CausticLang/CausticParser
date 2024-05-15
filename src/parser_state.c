#ifndef cap_PARSER_STATE_C_GUARD
#define cap_PARSER_STATE_C_GUARD 1

#include "parser_state.h"

#include <assert.h>
#include <malloc.h>

struct cap_ParserStack* cap_ps_push(struct cap_ParserState* state) {
    struct cap_ParserStack* prev = state->stack;
    state->stack = malloc(sizeof(struct cap_ParserStack));
    state->stack->prev = prev;
    state->stack->nodes = NULL;
    state->stack->nodecount = 0;
}
cst_index* cap_ps_pop(struct cap_ParserState* state) {
    assert(state->stack != NULL);
    cst_index* nodes = state->stack->nodes;
    struct cap_ParserStack* cur = state->stack;
    state->stack = state->stack->prev;
    free(cur);
}

#endif
