#include "../include/executor/executor.h"


TupleBuffer* tplbuffer;


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
        case (OP_AGGREGATE):
            op->getTuple = &aggregateGetTuple;
            break;
        default:
            printf("Don't know how to handle op-type %d\n", op->type);
            exit(1);
    }
}


void printTuple(Tuple* tpl) {



    char* printBuff = calloc(tpl->size, sizeof(char));

    for (size_t i = 0; i < tpl->columnCount; i++) {
        strcpy(printBuff + strlen(printBuff), getCol(tpl,i));
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


void execute(Operator *op) {

    if (op == NULL) {
        return;
    }

 
    tplbuffer = calloc(1, sizeof(TupleBuffer));
    tplbuffer->tuples = calloc(TUPLEBUFFSIZE, sizeof(Tuple));
    tplbuffer->bufferSize = TUPLEBUFFSIZE;

    tplbuffer->tupleCount = 0;

    doAssignGetTupleFunction(op);

    struct Tuple* tpl;
    
    if (op->resultDescription.columnCount == 0) {
        printf("No columns selected.");
        exit(1);
    }
    
    // Print column names
    printf("%s", op->resultDescription.columns[0].name);
    for (size_t i = 1; i < op->resultDescription.columnCount; i++) {
        printf("%c%s", DELIMITER, op->resultDescription.columns[i].name);
    }
    printf("\n");


    // Get tuples one by one
    for (;;) {
        tpl = getTuple(op->getTuple(op));
        if (tpl == NULL) break;

        printTuple(tpl);
    };

    free(tplbuffer->tuples);
    free(tplbuffer);
}
