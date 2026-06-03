#include "../../include/planner/planner.h"

Operator* makeInsert(Node* node) {
    /*
        Expected tree for INSERT 
        INSERT -> ident_tbl -> SELECT
    */

    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type = OP_STMTINSERT;

    strcpy(op->info.insert.targetTableName, node->next->content);
    
    
    /* Plan the query */
    Node* select = node->next->next;
    op->child = planQueryAst(select);
    
    /* Init colstats */
    op->info.insert.colStats = (ColumnStatistics*) calloc(op->child->resultDescription.columnCount, sizeof(ColumnStatistics));
    op->info.insert.colCount = op->child->resultDescription.columnOrderCount;

    for (size_t i = 0; i < op->info.insert.colCount; i++) {
        op->info.insert.colStats[i].min = calloc(1, COLSTATS_MINMAX_SIZE);
        op->info.insert.colStats[i].max = calloc(1, COLSTATS_MINMAX_SIZE);
    }

    printf("Allocated column stats for %ld columns\n", op->info.insert.colCount);


    return op;
}
