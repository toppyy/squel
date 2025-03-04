#include "../include/operators/project.h"

Tuple* projectGetTuple(Operator* op) {

    checkPtrNotNull(op->child, "OP_PROJECT has no child");
    checkPtrNotNull(op->child->getTuple, "Child of OP_PROJECT has no getTuple-method");

    /*
        Projection happens via the operator. The projection operator
        updates the result description to only include columns that
        were projected.

        This is an unfortunate extra function call :(
    */
    
    return op->child->getTuple(op->child);
}