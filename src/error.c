#include "error.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "parser_state.h"

/*unsigned int _cap_print_until(FILE* stream, char* start, char stop) {
    fprintf(stream, start);
    char c = fgetc(stream);
    unsigned int len = 0;
    for (; (c != stop) && (c != '\0'); len++) {
        fputc(c, stream);
        c = fgetc(stream);
        assert(len < 20);
    }
    return len;
}*/

void cap_error(struct cap_ParserState* state, cap_context_t* ctx, const char* rule, const char* msg, unsigned int start_pos, unsigned int end_pos) {
    if (!state->config.suppress_error_msgs) {
        unsigned int cno = cap_cno(state, end_pos);
        fprintf(cap_ERROR_STREAM, "Syntax error (%s) at l%u:c%u: %s\n", //----------------------------------------\n",
                rule, cap_lno(state), cno, msg);
        // Print source
        /// Print line before
        /*bool pre = false;
        unsigned int sol = start_pos;
        for (; sol > 0; sol--)
            if (ctx->buffer.buf[sol] == '\n') {
                pre = true;
                break;
            }
        if (pre) {
            unsigned int solp = sol;
            for (; solp > 0; solp--)
                if (ctx->buffer.buf[solp] == '\n') break;
            _cap_print_until(cap_ERROR_STREAM, ctx->buffer.buf+solp+1, '\n');
            fputc('\n', cap_ERROR_STREAM);
        }
        /// Print current line
        unsigned int eol = _cap_print_until(cap_ERROR_STREAM, ctx->buffer.buf+sol+1, '\n');
        //// Print context
        fprintf(cap_ERROR_STREAM, "\n$*c\n", cno, '^');
        /// Print line after
        _cap_print_until(cap_ERROR_STREAM, ctx->buffer.buf+eol+1, '\n');*/
    }
    if (!state->config.error_recover) exit(1);
}

void cap_debug(struct cap_ParserState* state, const int event, const char* rule, const int level, const int pos, const char* buffer, const int length) {
    assert(false); // TBA
}
