#include "../include/operators/project.h"

int projectGetTuple(Operator* op) {

    checkPtrNotNull(op->child, "OP_PROJECT has no child");
    checkPtrNotNull(op->child->getTuple, "Child of OP_PROJECT has no getTuple-method");
    
    int pooloffset = op->child->getTuple(op->child);

    if (pooloffset == -1) {
        return -1;
    }

    size_t sizes[ARRAYMAXSIZE];
    size_t  newcolumnCount = 0;
    size_t  newpCols[ARRAYMAXSIZE];
    size_t  newSize = 0;

    sizes[0] = op->child->resultDescription.pCols[0];
    for (size_t i = 1; i < op->child->resultDescription.columnCount; i++) {
        sizes[i] = op->child->resultDescription.pCols[0] - sizes[i-1];
    }

    
    size_t  j = 0;

    for (size_t i = 0; i < op->resultDescription.columnCount; i++) {

        j = op->info.project.colRefs[i];

        newpCols[newcolumnCount] = op->child->resultDescription.pCols[j];
        newcolumnCount++;
        newSize += sizes[j];

    }

    for (size_t i = 0; i < newcolumnCount; i++) {
        op->child->resultDescription.pCols[i] = newpCols[i];
    }

    op->resultDescription.columnCount = newcolumnCount;
    op->resultDescription.size = newSize;


    return pooloffset;
}