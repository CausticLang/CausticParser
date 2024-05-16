/*
    Deserializes nodes from STDIN and prints them to STDOUT
*/

#include <stdio.h>
#include <malloc.h>

#include "../parser.c"
#include "../src/ast/serialize.c"

int main(void) {
    struct cst_Root root;
    root.node_count = 0;
    root.nodes = NULL;
    cst_deserialize_to(&root, stdin);
    // Print nodes
    for (int i = 0; i < root.node_count; i++) {
        printf("Node@%d:\n", i);
        cst_print_node(root.nodes[i], " ", "\n");
    }
    // Cleanup
    for (int i = 0; i < root.node_count; i++) {
        cst_free_node(root.nodes[i]);
        free(root.nodes[i]);
    }
    free(root.nodes);
    return 0;
}
