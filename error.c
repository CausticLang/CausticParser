#include "error.h"

void cap_error(const char* rule, const char* msg, const char* input, unsigned int start_pos, unsigned int end_pos) {
    printf("ERR:\nrule=%s\nmsg=%s\ninput=%s\nstart_pos=%u\nend_pos=%u\n", rule, msg, input, start_pos, end_pos);
}
