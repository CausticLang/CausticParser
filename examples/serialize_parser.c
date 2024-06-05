/*
    Parses an AST from STDIN and serializes nodes to STDOUT
*/

#include <stdio.h>
#include <malloc.h>

#include "../parser_nowarn.c"
#include "../src/ast/utils/serialize.c"

int main(void) {
    // Setup root
    cst_Root* root = malloc(sizeof(cst_Root));
    *root = (cst_Root)cst_INITROOT;
    struct cap_ParserState* state = malloc(sizeof(struct cap_ParserState));
    state->root = root;
    state->config = (struct cap_ParserConfig)cap_DEFAULT_CONFIG;
    state->loc = (struct cap_ParserLoc)cap_DEFAULT_LOC;
    state->stack = NULL;
    // Setup parser and parse
    cap_context_t* ctx = cap_create(state);
    cst_index entry;
    cap_parse(ctx, &entry);
    // Serialize nodes
    cst_serialize_to(stdout, root);
    // Cleanup
    cap_destroy(ctx);
    free(state);
    for (size_t i = 0; i < root->n_nodes; i++) {
        cst_free_node(root->nodes[i]);
        free(root->nodes[i]);
    }
    free(root->nodes);
    free(root);
    return 0;
}
