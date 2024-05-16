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
    state->stack->mark = NONE;
}
void* cap_ps_pop(struct cap_ParserState* state) {
    assert(state->stack != NULL);
    void* nodes = state->stack->nodes;
    struct cap_ParserStack* cur = state->stack;
    state->stack = state->stack->prev;
    free(cur);
    return nodes;
}

unsigned int cap_lno(struct cap_ParserState* state) {
    return state->loc.line;
}
unsigned int cap_cno(struct cap_ParserState* state, unsigned int pos) {
    return pos - state->loc.line_at;
}

#endif
