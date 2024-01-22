#include "./include/parsetree.h"



Node* createNode(enum nodeType type, char* content) {
    struct Node * new = (Node*) malloc(sizeof(Node));
    new->type = type;
    strcpy(new->content, content);
    return new;
}

Node* createParsetree() {
    struct Node *root = createNode(ROOT,"");
    return root;
}

void addChild(Node *parent, enum nodeType type, char* content) {
    parent->child = createNode(type, content);
}

void addNext(Node *parent, enum nodeType type, char* content) {
    parent->next = createNode(type, content);
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