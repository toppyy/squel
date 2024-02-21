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
        op->resultDescription.columns[i].type = node->dtype;
        op->resultDescription.columns[i].identifier = node->identifier;
        strcpy(op->resultDescription.columns[i].name, node->content); 
        i++;
        node = node->next;        
        if (node == NULL) {
            break;
        }
    };

    op->resultDescription.columnCount = op->info.project.colCount;

    bool matched;
    for (i = 0; i < op->info.project.colCount; i++) {
        matched = false;
        for (size_t j = 0; j < child_op->resultDescription.columnCount; j++) {
            if (
                strcmp(op->info.project.columnsToProject[i], child_op->resultDescription.columns[j].name) == 0
            ) {
                op->info.project.colRefs[i] = j;
                op->resultDescription.columns[i].type = child_op->resultDescription.columns[j].type;
                matched = true;
                break;
            }
        }
        if (!matched) {
            printf("Making projection failed: Unable to find column '%s'\n", op->info.project.columnsToProject[i]);
            exit(1);
        }
    }

    return op;
}
