#include "../include/executor/executor.h"



TDB constructTDB(Node* node) {
    checkPtrNotNull(node, "NULL-ptr to constructTDB\n");
    if (node->type != COLUMNLIST) {
        printf("Cannot create a TDB without a COLUMNLIST\n");
        exit(1);
    }

    struct TDB tbl;
    Node* ptr_col = node->child;

    size_t colIdx = 0;
    while (ptr_col != NULL) {
        checkPtrNotNull(ptr_col->child, "TDB needs datatype of column (NULL-ptr)\n");
        
        tbl.lengths[colIdx] = sizeof(long);
        if (ptr_col->child->type == DTYPE_STR) {
            checkPtrNotNull(ptr_col->child->child, "DTYPE_STR needs length\n");
            tbl.lengths[colIdx] = atoi(ptr_col->child->child->content);
        }

        tbl.datatypes[colIdx] = ptr_col->child->type;

        ptr_col = ptr_col->next;
    }

    
    tbl.colCount = colIdx;
    tbl.records = 0;

    return tbl;
}


void executeCreateTable(Node* node) {


    /*
        Expected tree for CREATE TABLE
            CREATE -> TABLE -> ident -> column_list
                                        -> ident
                                            -> datatype
                                        -> ...

    */

    struct TDB tbl = constructTDB(node->next->next->next);

    strcpy(tbl.alias, node->next->next->content);


    char filepath[CHARMAXSIZE];
    char* ptr_filepath = filepath;
    sprintf(ptr_filepath, "%s/%s.%s", DATAPATH, tbl.alias, TDBFILEXT);

    printf("Creating a table %s\n", filepath);
    writeTdb(filepath, tbl);
}


void executeStatement(Node* node) {
    checkPtrNotNull(node, "NULL node passed to executeStatement");
    checkPtrNotNull(node->next, "Node passed to executeStatement has no next");

    switch (node->type) {
        case STMTCREATE:
            if (node->next->type != TABLE) {
                printf("Don't know how to create anything else than a table.\n");
                exit(1);
            }
            executeCreateTable(node);
            break;
        default:
            printf("Don't know how execute statement of type %d\n", node->type);
            exit(1);
    }
}