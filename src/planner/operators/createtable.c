#include "../../include/planner/planner.h"

Operator* makeCreateTable(Node* node) {

    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type    = OP_STMTCREATE;

    /*
        Expected tree for CREATE TABLE
            CREATE -> TABLE -> ident -> column_list
                                        -> ident
                                            -> datatype
                                        -> ...

    */

    strcpy(op->info.create.objectName, node->next->next->content);

    

    Node* ptr = node->next->next->next;

    if (ptr->type != COLUMNLIST) {
        printf("Expected a COLUMNLIST planning CREATE TABLE\n");
    }

    // Move to first column
    ptr = ptr->child;

    size_t colIndex = 0;

    while (ptr != NULL) {

        if (ptr->type != IDENT_COL) {
            printf("Expected a IDENT_COL when planning CREATE TABLE\n");
        }
        
        strcpy(op->info.create.columns[colIndex].name, ptr->content);

        Datatype type = mapParsedDatatypeToEnumDatatype(ptr->child->content);
        op->info.create.columns[colIndex].type = type;

        // For CHAR(), get the length 
        if (type == DTYPE_STR) {
            if (ptr->child->child->type != NUMBER) {
                printf("Expected a NUMBER when parsing CHAR-definition in CREATE TABLE\n");
            }
            op->info.create.columns[colIndex].size = atol(ptr->child->child->content);
        }

        colIndex++;

        ptr = ptr->next;
    }

    op->info.create.colCount = colIndex;


    return op;
}