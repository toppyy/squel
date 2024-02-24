#include "../include/operators/project.h"

Tuple* projectGetTuple(Operator* op) {

    checkPtrNotNull(op->child, "OP_PROJECT has no child");
    checkPtrNotNull(op->child->getTuple, "Child of OP_PROJECT has no getTuple-method");
    
    Tuple* tpl = op->child->getTuple(op->child);

    if (tpl == NULL) {
        return NULL;
    }
    
    size_t  newcolumnCount = 0;
    size_t  newSize = 0;
    char*   newpCols[ARRAYMAXSIZE];
    
    size_t  j = 0;

    for (size_t i = 0; i < op->resultDescription.columnCount; i++) {

        j = op->info.project.colRefs[i];

        newSize += strlen(tpl->pCols[j]) + 1;
        
        newpCols[newcolumnCount] = tpl->pCols[j];
        newcolumnCount++;

    }

    for (size_t i = 0; i < newcolumnCount; i++) {
        tpl->pCols[i] = newpCols[i];
    }

    tpl->columnCount = newcolumnCount;
    tpl->size = newSize;

    return tpl;

}