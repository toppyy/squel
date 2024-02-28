#pragma once
#include <stdlib.h>
#include <string.h>
#include "../const.h"

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
    DATATYPE,
    COLUMNLIST,
    AND,
    STMTCREATE,
    TABLE,
    OR
};

typedef struct Node {
    enum nodeType type;
    char content[CHARMAXSIZE];
    struct Node *next;
    struct Node *child;
    
    char alias[CHARMAXSIZE];
    char tblref[CHARMAXSIZE];

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