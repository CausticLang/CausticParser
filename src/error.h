#include <assert.h>

#include "parser_state.h"
#include "../parser.h"

void cap_error(struct cap_ParserState* state, cap_context_t* ctx, const char* rule, const char* msg, unsigned int start_pos, unsigned int end_pos);
void cap_debug(struct cap_ParserState* state, const int event, const char* rule, const int level, const int pos, const char* buffer, const int length);

#ifndef cap_ERROR_STREAM
    #define cap_ERROR_STREAM stderr
#endif
