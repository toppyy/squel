#pragma once
#include <stdlib.h>
#include <string.h>

typedef enum Datatype {
    DTYPE_STR,
    DTYPE_INT,
    DTYPE_UNDEFINED,
    DTYPE_LONG
} Datatype;


enum nodeType {
    EXPR,
    IDENT_COL,
    IDENT_TBL,
    FILEPATH,
    STRING,
    NUMBER,
    SELECT,
    IDENT_FUN,
    JOIN,
    STAR,
    FROM,
    WHERE,
    ON,
    ROOT,
    BOOLOP,
    ALIAS,
    AND,
    OR
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