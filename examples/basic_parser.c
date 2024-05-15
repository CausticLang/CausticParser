/*
    Parses an AST from STDIN and prints each node,
        subsequently freeing them
*/

#include <stdio.h>
#include <malloc.h>

#include "../parser.c"

int main() {
    // Setup root
    struct cst_Root* root = malloc(sizeof(struct cst_Root));
    root->nodes = NULL;
    root->node_count = 0;
    struct cap_ParserState* state = malloc(sizeof(struct cap_ParserState));
    state->root = root;
    state->stack = NULL;
    // Setup parser and parse
    cap_context_t* ctx = cap_create(state);
    cst_index* primary;
    cap_parse(ctx, primary);
    // Print nodes
    printf("Primary at index %d\n%d nodes\n", *primary, root->node_count);
    for (int i = 0; i < root->node_count; i++) {
        printf("Node@%d:\n", i);
        cst_print_node(root->nodes[i], " ", "\n");
    }
    // Cleanup
    cap_destroy(ctx);
    free(state);
    for (int i = 0; i < root->node_count; i++) {
        cst_free_node(root->nodes[i]);
        free(root->nodes[i]);
    }
    free(root->nodes);
    free(root);
}
