#include "../include/parser/parser.h"
#include "../include/parser/utils.h"

char rawSql[MAXQUERYSIZE];
int  qsize = 0;
int  cursor;
char nextChar;
bool nextToChild = false;

Node *root          = NULL;
Node *currentNode   = NULL;
size_t nodeCount    = 0;



void addNode(enum nodeType type, char* content) {
    
    nodeCount++;

    if (!nextToChild) {
        currentNode = addNext(currentNode, type, content);
        return;
    }

    currentNode = addChild(currentNode, type, content);
    nextToChild = false;
    
}



char getNextChar() {
    nextChar = rawSql[cursor++];
    return nextChar;
}

char expectChar(char expected) {
    if (nextChar == expected) 
        return getNextChar();

    printf("Was expecting '%c', but got '%c'.\n", expected, nextChar);
    exit(1);
}


bool peekWordMatches(char* w) {

    size_t i = 0;
    size_t start = cursor - 1;
    while (w[i++] == rawSql[start++]) {}
    if ((int) start > qsize) return false;
    return(i >= strlen(w));

}

char peekNextNonAlphanumeric() {
    int start = cursor - 1;
    while (isAlphaNumeric(rawSql[start++]) && start < qsize) {}
    if (start == qsize) return '\0';
    return rawSql[--start];
}


void expectWord(char* w) {

    size_t i = 0;
    int start = cursor;
    while (w[i++] == nextChar) {
        getNextChar();
    }

    if (i < strlen(w)) {
        printf("Expected to see %s at position %d (was %c)\n", w, start, rawSql[start]);
        exit(1);
    }
}


void keyword(char* kw, enum nodeType type) {

    expectWord(kw);
    addNode(type,kw);
}

void skipWhite() {
    while (isWhiteSpace(nextChar) & (cursor < qsize))
        getNextChar();
}


void filename() {
   
    char buff[MAXEXPRSIZE];
    memset(buff,0,MAXEXPRSIZE);
    int i = 0;
    while (nextChar != '\'') {
        buff[i++] = nextChar;
        getNextChar();
        if (i > qsize) {
            printf("Something went wrong :/ Cursor exceeds query length\n");
            exit(1);
        }
    }
    addNode(FILEPATH,buff);
}



void ident(enum nodeType type) {
   
    char buff[MAXEXPRSIZE];
    memset(buff,0,MAXEXPRSIZE);
    int i = 0;
    size_t tblrefCutOff = 0;
    while (isAlphaNumeric(nextChar) || nextChar=='_') {
        buff[i++] = nextChar;
        getNextChar();
        if (nextChar == '.') {
            getNextChar();
            tblrefCutOff = i;
        }
        if (i > qsize) {
            printf("Something went wrong :/ Cursor exceeds query length\n");
            exit(1);
        }
    }
    if (tblrefCutOff == 0) {
        addNode(type,buff);
        return;
    }

    addNode(type, &buff[tblrefCutOff]);
    buff[tblrefCutOff] = '\0';
    strcpy(currentNode->tblref, buff);    
}

void string() {
    char buff[MAXEXPRSIZE];
    memset(buff,0,MAXEXPRSIZE);
    int i = 0;
    expectChar('\'');
    while (nextChar != '\'') {
        buff[i++] = nextChar;
        getNextChar();
        if (i > qsize) {
            printf("Something went wrong :/ Cursor exceeds query length\n");
            exit(1);
        }
    }
    expectChar('\'');
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
    if (nextChar == '\'') {
        string();
        return;
    }
    number();    
}

void expr() {
    if (isLetter(nextChar)) {
        if (peekNextNonAlphanumeric() == '(') {
            ident(IDENT_FUN);
            nextToChild = true;
            expectChar('(');
            exprlist();
            expectChar(')');
            return;
        }
        ident(IDENT_COL);
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
        return;
    }

}
void boolExpr(bool expectOp);
void boolOp() {
    skipWhite();
    if (
        nextChar == '=' ||
        nextChar == '<' ||
        nextChar == '>'
    ) {
        char op[2] = { nextChar, '\0' };
        getNextChar();
        addNode(BOOLOP,op);
        boolExpr(false);
        return;
    }
    if (nextChar == '!') {
        getNextChar();
        expectChar('=');
        addNode(BOOLOP,"!=");
        boolExpr(false);
        return;
    }
    printf("Parser error: Expected a boolean operator. Got '%c'.\n", nextChar);
    exit(1);
}

void boolExpr(bool expectOp) {
    skipWhite();
    if (isLetter(nextChar)) {
        ident(IDENT_COL);
        if (expectOp) boolOp();
        return;
    }
    expr();
    if (expectOp) boolOp();
    return;
}

void boolExprList() {
    skipWhite();
    boolExpr(true);
    skipWhite();
    if (peekWordMatches("AND")) {
        keyword("AND",AND);
        boolExprList();
        return;
    }
    else if (peekWordMatches("OR")) {
        keyword("OR",OR);
        boolExprList();
        return;
    }
}

void addAlias(Node* node) {
    size_t i = 0;
    while (isLetter(nextChar)) {
        node->alias[i++] = nextChar;
        getNextChar();
    }
}


void query() {

    // Use tmp to jump back to the top level after collecting children
    // The tree structure will be
    // SELECT
    //      children..
    //      next: FROM
    // FROM
    //      children..
    //      next: WHERE(?)
    // WHERE
    //      children..

    Node* tmp = NULL;

    skipWhite();
    
    keyword("SELECT", SELECT);
    tmp = currentNode;
    skipWhite();
    nextToChild = true;
    exprlist();

    currentNode = tmp; // TODO: this is shit
    keyword("FROM", FROM);
    tmp = currentNode;
    nextToChild = true;
    skipWhite();
    from();
    skipWhite();

    currentNode = tmp;

    if (peekWordMatches("AS")) {
        getNextChar();
        getNextChar();
        skipWhite();
        addAlias(currentNode);
        skipWhite();
    }


    if (peekWordMatches("WHERE")) {

        keyword("WHERE", WHERE);
        nextToChild = true;
        skipWhite();
        boolExprList();
        skipWhite();
    }
}

void from() {
    skipWhite();
    source();
    skipWhite();
    if (peekWordMatches("JOIN")) {
        keyword("JOIN", JOIN);
        source();
        skipWhite();
        if (!peekWordMatches("ON")) {
            printf("Parser: Expected 'ON' specifying join keys\n");
            exit(1);
        }
        skipWhite();
        keyword("ON", ON);
        nextToChild = true;
        boolExprList();
        return;
    }
}

void source() {
    skipWhite();
    if (nextChar == '(') {
        getNextChar();
        query();
        expectChar(')');
        return;
    }
    if (nextChar == '\'') {
        getNextChar();
        filename();
        getNextChar();
        skipWhite();
        if (peekWordMatches("AS")) {
            getNextChar();
            getNextChar();
            skipWhite();
            addAlias(currentNode);
        }
        return;
    }

    ident(IDENT_TBL);
    skipWhite();
    if (peekWordMatches("AS")) {
        getNextChar();
        getNextChar();
        skipWhite();
        addAlias(currentNode);
    }

}

void getDatatype() {
    // Opportunistic reuse
    bool isChar = peekWordMatches("CHAR");
    ident(DATATYPE);
    skipWhite();
    if (isChar) {
        nextToChild = true;
        number();
    }
}

void columnDefinition() {
    skipWhite();
    ident(IDENT_COL);
    skipWhite();
    nextToChild = true;
    Node* tmp = currentNode;
    getDatatype();
    currentNode = tmp;
    skipWhite();
}

void columDefinitions() {
    skipWhite();
    columnDefinition();
    if (nextChar == ',') {
        getNextChar();        
        columDefinitions();
    }
    skipWhite();
}

void create() {
    keyword("CREATE", STMTCREATE);
    skipWhite();
    keyword("TABLE", TABLE);
    skipWhite();
    ident(IDENT_TBL);
    skipWhite();
    expectWord("AS");
    skipWhite();
    expectChar('(');
    addNode(COLUMNLIST, "list of columns");
    nextToChild = true;
    columDefinitions();
    expectChar(')');
}

void insert() {
    keyword("INSERT", STMTINSERT);
    skipWhite();
    expectWord("INTO");
    skipWhite();
    ident(IDENT_TBL);
    skipWhite();
    query();
}



void queryOrStmt() {

    if (peekWordMatches("CREATE")) {
        create();
        return;
    }

    if (peekWordMatches("INSERT")) {
        insert();
        return;
    }

    query();
}


void explain() {
    keyword("EXPLAIN", STMTEXPLAIN);
    skipWhite();
    queryOrStmt();
}


size_t parse(char* input, Node* p_root) {

    root = p_root;
    currentNode = p_root;

    strcpy(rawSql,input);
    qsize = strlen(rawSql);
    getNextChar();


    if (peekWordMatches("EXPLAIN")) {
        explain();
        return nodeCount;
    }

    if (peekWordMatches("CREATE")) {
        create();
        return nodeCount;
    }

    if (peekWordMatches("INSERT")) {
        insert();
        return nodeCount;
    }
    

    query();


    return nodeCount;   
}