#pragma once
#include <stdlib.h>
#include <string.h>

enum nodeType {
    EXPR,
    IDENT,
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
} Node;


Node* createParsetree();

Node* createNode(enum nodeType type, char* content);
void addChild(Node *parent, enum nodeType type, char* content);
void addNext(Node *parent, enum nodeType type, char* content);

void freeTree(Node *node);