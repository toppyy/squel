#include "../../include/planner/planner.h"


Operator* makeStarProjection(Operator* op, Operator* child_op) {

    for (size_t i = 0; i < child_op->resultDescription.columnCount; i++) {
        op->info.project.colRefs[i] = i;
        op->resultDescription.columns[i].type = child_op->resultDescription.columns[i].type;
        strcpy(op->resultDescription.columns[i].name, child_op->resultDescription.columns[i].name); 
        strcpy(op->info.project.columnsToProject[i], child_op->resultDescription.columns[i].name);
        op->info.project.colCount++;
    }

    op->resultDescription.columnCount = child_op->resultDescription.columnCount;

    
    return op;
}

Operator* makeProjectOp(Node* node, Operator* child_op) {

    if (child_op == NULL) {
        printf("Passed a NULL-pointer as child to makeProjectOp\n");
        exit(1);
    }


    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type    = OP_PROJECT;
    op->child   = NULL;

    /*
        SELECT * 

        TODO: how about SELECT *,col1 etc.?
    
    */
    if (node->type == STAR) {
        return makeStarProjection(op, child_op);
    }

    /*  We know the result set of the child of project op. So we can just which indexes match 
        the projected columns
    */

    int i = 0;

    for (;;) {
        
        op->info.project.colCount++;
        
        strcpy(op->info.project.columnsToProject[i], node->content);
        strcpy(op->resultDescription.columns[i].name, node->content);

        op->resultDescription.columns[i].type = node->dtype;
        op->resultDescription.columns[i].identifier = node->identifier;
        
        // Find the index of the projected column in the result description
        // of the child
        int j = findColIdxInResDesc(&child_op->resultDescription, node->content, node->tblref);
        op->info.project.colRefs[i] = j;
        op->resultDescription.columns[i].type = child_op->resultDescription.columns[j].type;

        i++;
        node = node->next;        
        if (node == NULL) {
            break;
        }
    };

    op->resultDescription.columnCount = op->info.project.colCount;

    return op;
}
