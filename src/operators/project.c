#include "../include/operators/project.h"

int projectGetTuple(Operator* op) {

    checkPtrNotNull(op->child, "OP_PROJECT has no child");
    checkPtrNotNull(op->child->getTuple, "Child of OP_PROJECT has no getTuple-method");
    
    Tuple* tpl = getTuple(op->child->getTuple(op->child));

    if (tpl == NULL) {
        return -1;
    }
    
    size_t  newcolumnCount = 0;
    size_t  newpCols[ARRAYMAXSIZE];
    
    size_t  j = 0;

    for (size_t i = 0; i < op->resultDescription.columnCount; i++) {

        j = op->info.project.colRefs[i];

        newpCols[newcolumnCount] = tpl->pCols[j];
        newcolumnCount++;

    }

    for (size_t i = 0; i < newcolumnCount; i++) {
        tpl->pCols[i] = newpCols[i];
    }

    tpl->columnCount = newcolumnCount;

    return tpl->idx;
}