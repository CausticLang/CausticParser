#include "error.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "parser_state.h"

void cap_error(struct cap_ParserConfig* conf, const char* rule, const char* msg, const char* input, unsigned int start_pos, unsigned int end_pos) {
    if (!conf->suppress_error_msgs) {
        fprintf(cap_ERROR_STREAM, "Syntax error (%s): %s\n%s\n%*c", rule, msg, input, start_pos, '^');
        if (start_pos == end_pos) fputc('\n', cap_ERROR_STREAM);
        else fprintf(cap_ERROR_STREAM, "%*c\n", end_pos-start_pos, '^');
    }
    if (!conf->error_recover) exit(1);
}

void cap_debug(struct cap_ParserState* state, const int event, const char* rule, const int level, const int pos, const char* buffer, const int length) {
    assert(false); // TBA
}
