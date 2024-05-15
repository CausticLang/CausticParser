#include "error.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "parser_state.h"

void cap_error(struct cap_ParserState* state, const char* rule, const char* msg, const char* input, unsigned int start_pos, unsigned int end_pos) {
    if (!state->config.suppress_error_msgs) {
        fprintf(cap_ERROR_STREAM,
                "Syntax error (%3$s) at l%4$u:c%1$u: %5$s\n----------------------------------------\n%6$s\n%2$*c\n",
                cap_cno(state, start_pos), '^', rule, cap_lno(state), msg, input);
    }
    if (!state->config.error_recover) exit(1);
}

void cap_debug(struct cap_ParserState* state, const int event, const char* rule, const int level, const int pos, const char* buffer, const int length) {
    assert(false); // TBA
}
