#include "nodes.h"

#define cst_DESERIALIZE_CHAR_BUFF_SIZE 16

void cst_serialize_to(struct cst_Root* root, FILE* stream);

void cst_nserialize_to(struct cst_NodeBase* node, FILE* stream);
void cst_nserialize_metadata_to(struct cst_NodeBase* node, FILE* stream);


void cst_deserialize_to(struct cst_Root* root, FILE* stream);

struct cst_NodeBase* cst_ndeserialize_from(FILE* stream);
