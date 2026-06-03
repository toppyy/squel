#include "../../include/planner/planner.h"


Operator* makeStarProjection(Operator* op, Operator* child_op) {

    for (size_t i = 0; i < child_op->resultDescription.columnCount; i++) {
        op->resultDescription.columns[i].active =  child_op->resultDescription.columns[i].active;
        op->resultDescription.columns[i].type = child_op->resultDescription.columns[i].type;
        op->resultDescription.columnOrder[i] = i;
        strcpy(op->resultDescription.columns[i].name, child_op->resultDescription.columns[i].name); 
    }

    op->resultDescription.columnCount = child_op->resultDescription.columnCount;
    op->resultDescription.columnOrderCount = op->resultDescription.columnCount;

    
    return op;
}

Operator* makeProjectOp(Node* node, Operator* child_op) {

    checkPtrNotNull(child_op,"Passed a NULL-pointer as child to makeProjectOp.");

    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type    = OP_PROJECT;
    op->child   = NULL;

    /*
        SELECT * 

        TODO: how about SELECT *,col1 etc.?
    
        */
    makeStarProjection(op, child_op);

    
    if (node->type == STAR) {
        return op;
    }
    
    /* Deactivate all of the child columns. We'll mark them active if they are in the projection. */
    for (size_t i = 0; i < op->resultDescription.columnCount; i++) {
        op->resultDescription.columns[i].active = 0;
        op->resultDescription.columnOrder[i] = 0;
    } 


    /*  We know the result set of the child of project op. So we can just which indexes match 
        the projected columns
    */
    size_t order = 0;

    for (;;) {
        
        // Find the index of the projected column in the result description
        // of the child
        size_t j = findColIdxInResDesc(&child_op->resultDescription, node->content, node->tblref);

        op->resultDescription.columns[j].active  = 1;
        op->resultDescription.columnOrder[order] = j;

        order++;

        node = node->next;

        if (node == NULL) {
            break;
        }

    };

    op->resultDescription.columnOrderCount = order;

        /* Init colstats */
    op->colStats = (ColumnStatistics*) calloc(op->resultDescription.columnOrderCount, sizeof(ColumnStatistics));

    long max_long = LONG_MAX;
    printf("Max long is %ld\n", max_long);

    for (size_t i = 0; i < op->resultDescription.columnOrderCount; i++) {
        op->colStats[i].min = calloc(COLSTATS_ROW_GROUPS, COLSTATS_MINMAX_SIZE);
        op->colStats[i].max = calloc(COLSTATS_ROW_GROUPS, COLSTATS_MINMAX_SIZE);

        for (size_t z = 0; z < COLSTATS_ROW_GROUPS; z++)  memcpy(op->colStats[i].min + (z * sizeof(long)), &max_long, sizeof(max_long));
    }

    printf("Allocated column stats for %ld columns\n", op->info.insert.colCount);


    

    return op;
}
