#include "./include/parser.h"
#include "./include/utils.h"

char rawSql[MAXQUERYSIZE];
int  qsize = 0;
char nextChar;
int  cursor;

Node *nodes = NULL;
size_t nodeCount    = 0;
size_t nodeBuffSize = NODEBUFFSIZE;

void addNode(enum nodeType type, char* content) {
    nodes[nodeCount].type = type;
    strcpy(nodes[nodeCount].content,content);
    nodeCount++;
    if (nodeCount == nodeBuffSize) {
        printf("Nodebuffer is full!");
        exit(1);

    }
}

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


void keyword(char* kw, enum nodeType type) {

    size_t i = 0;
    int start = cursor;
    while (kw[i++] == nextChar) {
        getNextChar();
    }

    if (i < strlen(kw)) {
        printf("Expected to see %s at position %d (was %c)", kw, start, rawSql[start]);
        exit(1);
    }
    addNode(type,kw);
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
    addNode(IDENT,buff);
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
    addNode(STRING,buff);
}

void expectNumber() {
    if (!isNumeric(nextChar)) {
        printf("Expected a number, got: %c.\n", nextChar);
        exit(1);
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
    addNode(NUMBER,buff);
}

void constant() {
    if (nextChar == '"') {
        string();
        return;
    }
    number();    
}

void expr() {
    if (isLetter(nextChar)) {
        ident();
        return;
    }
    if (nextChar == '*') {
        expectChar('*');
        addNode(STAR,"*");
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
    keyword("SELECT", SELECT);
    skipWhite();
    exprlist();
    keyword("FROM", FROM);
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



size_t parse(char* input, Node **nodeOutput __attribute__ ((unused))) {

    nodes = *nodeOutput;


    strcpy(rawSql,input);
    qsize = strlen(rawSql);
    getNextChar();
    query();
    
    return nodeCount;   
}