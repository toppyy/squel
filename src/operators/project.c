#include "../include/operators/project.h"

Tuple* projectGetTuple(Operator* op) {

    if (op->child == NULL) {
        printf("OP_PROJECT has no child\n");
        exit(1);
    }

    if ( op->child->getTuple == NULL) {
        printf("Child of OP_PROJECT has no getTuple-method\n");
        exit(1);
    }
    
    Tuple* tpl = op->child->getTuple(op->child);

    if (tpl == NULL) {
        return NULL;
    }

    
    size_t  newcolumnCount = 0;
    size_t  newidentifiers[TUPLECOLS];
    size_t  newSize = 0;
    char*   newpCols[TUPLECOLS];
    
    size_t  j = 0;

    for (size_t i = 0; i < op->resultDescription.columnCount; i++) {

        j = op->info.project.colRefs[i];

        newSize += strlen(tpl->pCols[j]) + 1;
        
        newidentifiers[newcolumnCount] = tpl->identifiers[j];
        newpCols[newcolumnCount] = tpl->pCols[j];
        newcolumnCount++;

    }

    for (size_t i = 0; i < newcolumnCount; i++) {
        tpl->identifiers[i] = newidentifiers[i];
        tpl->pCols[i] = newpCols[i];
    }

    tpl->columnCount = newcolumnCount;
    tpl->size = newSize;

    return tpl;

}