#include "../include/executor/executor.h"
#include "../include/executor/tuple.h"



void assignGetTupleFunction(Operator *op) {

    if (op == NULL) {
        printf("Passed a null pointer to assignGetTupleFunction\n");
        exit(1);
    }
    switch(op->type) {
        case(OP_SCAN):
            op->getTuple = &scanGetTuple;
            break;
        case(OP_SCANTDB):
            op->getTuple = &scanTDBGetTuple;
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
        case (OP_HASHJOIN):
            op->getTuple = &hashjoinGetTuple;
            break;
        case (OP_AGGREGATE):
            op->getTuple = &aggregateGetTuple;
            break;
        default:
            printf("EXECUTOR-error: Don't know how to handle op-type %d\n", op->type);
            exit(1);
    }
}


void doAssignGetTupleFunction(Operator* p_op) {

    if (p_op == NULL) {
        return;
    }

    assignGetTupleFunction(p_op);

    if (p_op->child != NULL) {
        doAssignGetTupleFunction(p_op->child);
    }

    if (p_op->type == OP_JOIN || p_op->type == OP_HASHJOIN) {
        doAssignGetTupleFunction(p_op->info.join.left);
        doAssignGetTupleFunction(p_op->info.join.right);
    }
}


void execute(Operator* op, bool printColNames, void (*tupleHandler)(Tuple* tpl)) {

    if (op == NULL) {
        return;
    }

 
    doAssignGetTupleFunction(op);

    if (op->resultDescription.columnCount == 0) {
        printf("No columns selected.");
        exit(1);
    }
    
    // Print column names
    if (printColNames) {
        printf("%s", op->resultDescription.columns[0].name);
        for (size_t i = 1; i < op->resultDescription.columnCount; i++) {
            printf("%c%s", DELIMITER, op->resultDescription.columns[i].name);
        }
        printf("\n");

    }

    // Get tuples one by one
    Tuple* tpl = initTupleOfSize(TUPLESIZE);
    for (;;) {
        op->getTuple(op, tpl);
        if (isTupleEmpty(tpl)) break;

        tupleHandler(tpl);

    };
    freeTuple(tpl);


}
