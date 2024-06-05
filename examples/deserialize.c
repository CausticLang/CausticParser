/*
    Deserializes nodes from STDIN and prints them to STDOUT
*/

#include <stdio.h>
#include <malloc.h>

#include "../parser_nowarn.c"
#include "../src/ast/utils/serialize.c"

int main(void) {
    cst_Root* root = cst_deserialize_from(stdin);
    // Print nodes
    for (size_t i = 0; i < root->n_nodes; i++) {
        printf("Node@%zu:\n", i);
        cst_print_node(stdout, root->nodes[i]);
    }
    // Cleanup
    for (int i = 0; i < root->n_nodes; i++) {
        cst_free_node(root->nodes[i]);
        free(root->nodes[i]);
    }
    free(root->nodes);
    free(root);
    return 0;
}
