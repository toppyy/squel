#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "./parsetree.h"

#define MAXQUERYSIZE 1000
#define MAXEXPRSIZE  100
#define NODEBUFFSIZE 10

char getNextChar();
char expectChar(char expected);
void keyword(char* kw, enum nodeType type);
void skipWhite();
void ident();
void string();
void number();
void constant();
void expr();
void exprlist();
void query();
void source();
size_t parse(char* input, Node **nodeOutput);


#endif  // PARSER_H