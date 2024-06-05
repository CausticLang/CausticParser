/*
    Parses an AST from STDIN and prints each node,
        subsequently freeing them
*/

#include <stdio.h>
#include <malloc.h>

#include "../parser_nowarn.c"

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
    int rval = cap_parse(ctx, &entry);
    // Print nodes
    printf("cap_parse() rval: %d\nEntrypoint at index %u\n%zu node(s)\n", rval, entry, root->n_nodes);
    for (size_t i = 0; i < root->n_nodes; i++) {
        printf("Node@%zu:\n", i);
        cst_print_node(stdout, root->nodes[i]);
    }
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
