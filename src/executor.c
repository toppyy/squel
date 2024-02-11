#include "./include/executor/executor.h"


TupleBuffer* tplbuffer;


char *buffercache;
char *p_buffercache;
char *bufferscan;
size_t buffercacheSize = 0;


void assignGetTupleFunction(Operator *op) {

    if (op == NULL) {
        printf("Passed a null pointer to assignGetTupleFunction\n");
        exit(1);
    }
    switch(op->type) {
        case(OP_SCAN):
            op->getTuple = &scanGetTuple;
            break;
        case(OP_PROJECT):
            op->getTuple = &projectGetTuple;
            break;
        case (OP_FILTER):
            op->getTuple = &filterGetTuple;
            break;
        case (OP_JOIN):
            op->getTuple = &joinGetTuple;
            break;
        default:
            printf("Don't know how to handle op-type %d\n", op->type);
            exit(1);
    }
}


void printTuple(Tuple* tpl) {


    char* printBuff = calloc(tpl->size, sizeof(char));

    for (size_t i = 0; i < tpl->columnCount; i++) {
        strcpy(printBuff + strlen(printBuff), tpl->pCols[i]);
        if (i == tpl->columnCount - 1) continue;
        strcpy(printBuff + strlen(printBuff), DELIMITERSTR);
    }

    printf("%s\n", printBuff);

    free(printBuff);
}


void doAssignGetTupleFunction(Operator* p_op) {

    if (p_op == NULL) {
        return;
    }

    assignGetTupleFunction(p_op);

    if (p_op->child != NULL) {
        doAssignGetTupleFunction(p_op->child);
    }

    if (p_op->type == OP_JOIN) {
        doAssignGetTupleFunction(p_op->info.join.left);
        doAssignGetTupleFunction(p_op->info.join.right);
    }
}

void printBuffercache() {
    size_t i = 0;
    char prev = 'a', cur = 'a';
    while (i < QUERYBUFFER) {
        prev = cur;   
        cur = p_buffercache[i++];
        if (prev == '\0' && cur == '\0') {
            continue;
        }
        if (cur == '\0') {
            printf("\n");
            continue;
        }
        printf("%c",cur);
    }
}

void execute(Operator *op) {

    p_buffercache = calloc(QUERYBUFFER, sizeof(char)); 
    bufferscan  = calloc(SCANBUFFER, sizeof(char)); 
    buffercache = p_buffercache;

    tplbuffer = calloc(1, sizeof(TupleBuffer));

    tplbuffer->tupleCount = 0;

    doAssignGetTupleFunction(op);

    struct Tuple* tpl;


    for (;;) {
        tpl = op->getTuple(op);
        if (tpl == NULL) break;

        printTuple(tpl);
    };

    free(p_buffercache);
    free(bufferscan);
    free(tplbuffer);
}
