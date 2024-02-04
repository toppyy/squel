#include "../include/parser/parsetree.h"



Node* createNode(enum nodeType type, char* content) {
    struct Node * new = (Node*) calloc(1,sizeof(Node));
    new->type = type;
    strcpy(new->content, content);
    new->child  = NULL;
    new->next   = NULL;

    /* These are used by the binder */
    new->tableRef = -1;
    new->colRef   = -1;
    new->dtype    = DTYPE_UNDEFINED;
    new->identifier = -1;
    
    return new;
}

Node* createParsetree() {
    struct Node *root = createNode(ROOT,"root");
    return root;
}

Node* addChild(Node *parent, enum nodeType type, char* content) {
    parent->child = createNode(type, content);
    return parent->child;
}

Node* addNext(Node *parent, enum nodeType type, char* content) {
    parent->next = createNode(type, content);
    return parent->next;
}


void freeTree(Node *node) {

    if (node->child != NULL) {
        freeTree(node->child);
    }

    if (node->next != NULL) {
        freeTree(node->next);
    }

    free(node);
}