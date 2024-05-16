/*
    Parses an AST from STDIN and serializes nodes to STDOUT
*/

#include <stdio.h>
#include <malloc.h>

#include "../parser.c"
#include "../src/ast/serialize.c"

int main(void) {
    // Setup root
    struct cst_Root* root = malloc(sizeof(struct cst_Root));
    root->nodes = NULL;
    root->node_count = 0;
    struct cap_ParserState* state = malloc(sizeof(struct cap_ParserState));
    state->root = root;
    state->config = (struct cap_ParserConfig)cap_DEFAULT_CONFIG;
    state->loc = (struct cap_ParserLoc)cap_DEFAULT_LOC;
    state->stack = NULL;
    // Setup parser and parse
    cap_context_t* ctx = cap_create(state);
    cst_index* primary;
    cap_parse(ctx, primary);
    // Serialize nodes
    cst_serialize_to(root, stdout);
    // Cleanup
    cap_destroy(ctx);
    free(state);
    for (int i = 0; i < root->node_count; i++) {
        cst_free_node(root->nodes[i]);
        free(root->nodes[i]);
    }
    free(root->nodes);
    free(root);
    return 0;
}
