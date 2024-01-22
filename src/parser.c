#include "./include/parser.h"
#include "./include/utils.h"

char rawSql[MAXQUERYSIZE];
int  qsize = 0;
char nextChar;
int  cursor;


char* KEYWORDS[2] = {
    "SELECT",
    "FROM"
};

bool isKeyword(char* w) {

    for (size_t i = 0; i < SIZEOF(KEYWORDS); i++) {

        if ( strcmp(w,KEYWORDS[i]) == 0) {
            printf("Matched: %s\n", KEYWORDS[i]);
            return true;
        }

    }

    return false;
}

char getNextChar() {
    nextChar = rawSql[cursor++];
    return nextChar;
}

char expectChar(char expected) {
    if (nextChar == expected) 
        return getNextChar();

    printf("Was expecting %c, but got %c.\n", expected, nextChar);
    exit(1);
}


void keyword(char* kw) {

    size_t i = 0;
    int start = cursor;
    while (kw[i++] == nextChar) {
        getNextChar();
    }

    if (i < strlen(kw)) {
        printf("Expected to see %s at position %d (was %c)", kw, start, rawSql[start]);
        exit(1);
    }

    printf("Parsed %s\n", kw);
}

void skipWhite() {
    while (isWhiteSpace(nextChar))
        getNextChar();
}


void ident() {
   
    char buff[MAXEXPRSIZE];
    memset(buff,0,MAXEXPRSIZE);
    int i = 0;
    while (isAlphaNumeric(nextChar)) {
        buff[i++] = nextChar;
        getNextChar();
        if (i > qsize) {
            printf("Something went wrong :/ Cursor exceeds query length\n");
            exit(1);
        }
    }

    printf("Parsed ident %s (of size %d)\n", buff, i);
}

void string() {
    char buff[MAXEXPRSIZE];
    memset(buff,0,MAXEXPRSIZE);
    int i = 0;
    expectChar('"');
    while (nextChar != '"') {
        buff[i++] = nextChar;
        getNextChar();
        if (i > qsize) {
            printf("Something went wrong :/ Cursor exceeds query length\n");
            exit(1);
        }
    }
    expectChar('"');
    printf("Parsed string constant %s (of size %d)\n", buff, i);
}

void expectNumber() {
    if (!isNumeric(nextChar)) {
        printf("Expected a number, got: %c.\n", nextChar);
    }
}

void number() {
    char buff[MAXEXPRSIZE];
    memset(buff,0,MAXEXPRSIZE);
    int i = 0;
    
    expectNumber();

    while (isNumeric(nextChar)) {
        buff[i] = nextChar;
        getNextChar();
        i++;
        if (i > qsize) {
            printf("Something went wrong :/ Cursor exceeds query length\n");
            exit(1);
        }
    }
    printf("Parsed a number constant %s (of size %d)\n", buff, i);
}

void constant() {
    if (nextChar == '"') {
        string();
        return;
    }
    // If it's not a string, it must be a number
    number();
    
}

void expr() {
    if (isLetter(nextChar)) {
        ident();
        return;
    }
    constant();    
}

void exprlist() {
    skipWhite();
    expr();
    skipWhite();
    if (nextChar == ',') {
        getNextChar();
        exprlist();
    }
}

void query() {
    skipWhite();
    keyword("SELECT");
    skipWhite();
    exprlist();
    keyword("FROM");
    skipWhite();
    source();
}

void source() {
    skipWhite();
    if (nextChar == '(') {
        getNextChar();
        query();
        expectChar(')');
        return;
    }
    ident();
}   



void printToken(struct Token tkn) {
    printf("Token start %d, end %d\n", tkn.start, tkn.end);
}




struct Token nextToken(char *rawSql, int n, int start) {
    int i = start;
    struct Token tkn;

    while (isWhiteSpace(rawSql[i]) && i < n ) {
        i++;
    };

    tkn.start = i;
    i++;

    while (isAlphaNumeric(rawSql[i]) && i < n ) {
        i++;
    };

    tkn.end = i;
    return tkn;
}


void parse(char* input) {
    strcpy(rawSql,input);
    qsize = strlen(rawSql);
    getNextChar();
    query();
}