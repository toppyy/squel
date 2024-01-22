#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXQUERYSIZE 1000
#define MAXEXPRSIZE  100

char rawSql[MAXQUERYSIZE];
int  qsize = 0;
char nextChar;
int  cursor;

char getNextChar();
char expectChar(char expected);
void parseKeyword(char* kw);
void skipWhite();
void ident();
void string();
void number();
void constant();
void expr();
void exprlist();
void query();
void source();

struct Token {
    int start;
    int end;
    char content[50];
};


struct Token nextToken(char *rawSql, int n, int start);
void printToken(struct Token tkn);

