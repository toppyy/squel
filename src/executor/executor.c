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
            printf("EXECUTOR-error: Don't know how to assign getTuple for op-type %d\n", op->type);
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


void executeSelect(Operator* op, void (*tupleHandler)(Tuple* tpl)) {

    checkPtrNotNull(op, "Internal error: NULL-ptr passed to execute");

    doAssignGetTupleFunction(op);

    if (op->resultDescription.columnCount == 0) {
        printf("No columns selected.");
        exit(1);
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


void execute(Operator* op, void (*tupleHandler)(Tuple* tpl)) {

    switch(op->type) {
        case OP_STMTCREATE:
            executeCreateTable(op);
            break;
        case OP_STMTEXPLAIN:
            executeExplain(op->child);
            break;
        case OP_STMTINSERT:
            executeInsert(op);
            break;
        default:
            executeSelect(op, tupleHandler);
    }

}
