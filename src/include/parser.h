#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "./parsetree.h"

#define MAXQUERYSIZE 1000
#define MAXEXPRSIZE  100


char getNextChar();
char expectChar(char expected);
void keyword(char* kw);
void skipWhite();
void ident();
void string();
void number();
void constant();
void expr();
void exprlist();
void query();
void source();
void parse(char* input, Node *parsetree);

struct Token {
    int start;
    int end;
    char content[50];
};


struct Token nextToken(char *rawSql, int n, int start);
void printToken(struct Token tkn);

#endif  // PARSER_H