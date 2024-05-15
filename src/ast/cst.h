#ifndef cst_CST_H_GUARD
#define cst_CST_H_GUARD 1

#include "nodes.h"

void cst_free_node(struct cst_NodeBase* node);
void cst_print_node(struct cst_NodeBase* node, char* prefix, char* suffix);

#endif
