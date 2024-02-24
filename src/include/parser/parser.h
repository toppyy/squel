#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../const.h"
#include "./parsetree.h"
#include "./utils.h"

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
void filename();
void source();
void from();
void function();
size_t parse(char* input, Node *p_root);