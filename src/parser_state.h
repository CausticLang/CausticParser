#ifndef cap_PARSER_STATE_H_GUARD
#define cap_PARSER_STATE_H_GUARD 1

#include "ast/cst.h"

#include <stdbool.h>

struct cap_ParserConfig {
    bool suppress_error_msgs;
    bool error_recover;
    bool track_comments;
    bool track_empty_lines;
};
#define cap_DEFAULT_CONFIG { \
    .suppress_error_msgs = false, \
    .error_recover = false, \
    .track_comments = false, \
    .track_empty_lines = false, \
}
struct cap_ParserLoc {
    unsigned int line;
    unsigned int line_at;
};
#define cap_DEFAULT_LOC { \
    .line = 0, \
    .line_at = 0, \
}

struct cap_ParserState {
    struct cst_Root* root;
    struct cap_ParserConfig config;
    struct cap_ParserLoc loc;
    struct cap_ParserStack* stack;
};

enum cap_ParserStackMark {
    NONE,
    BLOCK,
    INVOKE_ARGS, INVOKE_KWARGS, PROC_PARAMS,
    ENUM_BODY, STRUCT_BODY, STRUCTENUM_BODY, CLASS_BODY,
};

struct cap_ParserStack {
    struct cap_ParserStack* prev;
    void* data;
    unsigned int auxcount;
    enum cap_ParserStackMark mark;
};


struct cap_ParserStack* cap_ps_push(struct cap_ParserState* state);
void* cap_ps_pop(struct cap_ParserState* state);

unsigned int cap_lno(struct cap_ParserState* state);
unsigned int cap_cno(struct cap_ParserState* state, unsigned int pos);


#endif
