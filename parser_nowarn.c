// Disables warnings for unused labels in generated `parser.c`

#ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-label"
#elif __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wunused-label"
#else
    #warning "Unknown compiler, label warnings may be emitted"
#endif

#include "parser.c"

#ifdef __GNUC__
    #pragma GCC diagnostic pop
#elif __clang__
    #pragma clang diagnostic pop
#endif
