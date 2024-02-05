#include "./include/planner/planner.h"

void freeQueryplan(Operator *node) {

    if (node->child != NULL) {
        freeQueryplan(node->child);
    }

    free(node);
}


Operator* makeScanOp(TableMetadata tbl) {

    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type = OP_SCAN;
    memcpy(&op->info.scan.table, &tbl, sizeof(tbl));
    op->next                    = NULL;
    op->child                   = NULL;
    op->info.scan.tablefile     = NULL;
    op->info.scan.cursor        = 0;
    op->getTuple                = NULL;

    for (size_t i = 0; i < tbl.columnCount; i++) {
        op->resultDescription.columns[i].type = tbl.columns[i].type;
        op->resultDescription.columns[i].identifier = tbl.columns[i].identifier;

    }
    op->resultDescription.columnCount = tbl.columnCount;

    return op;
}



Operator* makeFilterOp(Node* node, Operator* child) {

    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type = OP_FILTER;
    op->next                    = NULL;
    op->child                   = NULL;
    op->getTuple                = NULL;

    memcpy(&op->resultDescription, &child->resultDescription, sizeof(child->resultDescription));

    return op;
}


Operator* makeProjectOp(Node* node) {

    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type = OP_PROJECT;
    op->next    = NULL;
    op->child   = NULL;

    int i = 0;

    while (node->next != NULL)  {
        
        op->info.project.colCount++;
        strcpy(op->info.project.columnsToProject[i], node->content);
        op->info.project.colRefs[i] = node->colRef;
        op->info.project.tblRefs[i] = node->tableRef;
        op->resultDescription.columns[i].type = node->dtype;
        op->resultDescription.columns[i].identifier = node->identifier;
        i++;
        node = node->next;

        
    } ;


    // TODO. The last column is not handled by the loop, so 
    // the loop body is repeated
    op->info.project.colCount++;
    strcpy(op->info.project.columnsToProject[i], node->content);
    op->info.project.colRefs[i] = node->colRef;
    op->info.project.tblRefs[i] = node->tableRef;
    op->resultDescription.columns[i].type = node->dtype;
    op->resultDescription.columns[i].identifier = node->identifier;
    i++;

    op->resultDescription.columnCount = op->info.project.colCount;


    return op;
}




Operator* planQuery(Node* astRoot, TableMetadata* tables, size_t tableCount) {

    /* 
        Building the QueryPlan which is a tree of operators.
        The tree is built from the ground up. 

        - Each reference to a physical table is a leaf node
        - The root if the tree is always an project operation
    
    */

    Node* SELECT = astRoot->next;
    Operator* op_proj = makeProjectOp(SELECT->child);

    Operator* op_scan = makeScanOp(tables[0]);

    Node* WHERE = astRoot->next->next->next;
    if (WHERE != NULL) {
        Operator* op_filt = makeFilterOp(WHERE, op_scan);
        op_scan->next   = op_filt;
        op_filt->next   = op_proj;

        op_proj->child  = op_filt;
        op_filt->child  = op_scan;
    } else {
        op_scan->next  = op_proj;
        op_proj->child = op_scan;
    }


    return op_proj;   
}