#pragma once
#include <stdlib.h>
#include <string.h>

typedef enum {
    DTYPE_STR,
    DTYPE_INT,
    DTYPE_UNDEFINED
} Datatype;


enum nodeType {
    EXPR,
    IDENT_COL,
    IDENT_TBL,
    FILEPATH,
    STRING,
    NUMBER,
    SELECT,
    STAR,
    FROM,
    WHERE,
    ROOT
};

typedef struct Node {
    enum nodeType type;
    char content[50];
    struct Node *next;
    struct Node *child;

    /* These are used by the binder */
    int tableRef;
    int colRef;
    Datatype dtype;
    size_t identifier;

} Node;


Node* createParsetree();

Node* createNode(enum nodeType type, char* content);
Node* addChild(Node *parent, enum nodeType type, char* content);
Node* addNext(Node *parent, enum nodeType type, char* content);

void freeTree(Node *node);