#include "error.h"

#include <stdio.h>
#include <stdlib.h>

#include "parser_state.h"

void cap_error(struct cap_ParserConfig* conf, const char* rule, const char* msg, const char* input, unsigned int start_pos, unsigned int end_pos) {
    if (!conf->suppress_error_msgs)
        fprintf(cap_ERROR_STREAM, cap_ERROR_FMT, rule, msg, input, start_pos, end_pos);
    if (!conf->error_recover)
        exit(1);
}

void cap_debug(struct cap_ParserState* state, const char* event, const char* rule, const int event, const char* buffer, const int length)
