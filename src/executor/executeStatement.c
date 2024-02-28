#include "../include/executor/executor.h"


Datatype mapParsedDatatypeToEnumDatatype(char* parsed) {


    if (strcmp(parsed, "CHAR") == 0) return DTYPE_STR;
    if (strcmp(parsed, "INT") == 0) return DTYPE_LONG;


    printf("Don't know how map nodetype %s to datatype\n", parsed);
    exit(1);
}


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
        Datatype dtype = mapParsedDatatypeToEnumDatatype(ptr_col->child->content);

        strcpy(tbl.colNames[colIdx], ptr_col->content);
        tbl.datatypes[colIdx] = DTYPE_STR;
        tbl.lengths[colIdx] = sizeof(long);
        if (dtype == DTYPE_STR) {
            checkPtrNotNull(ptr_col->child->child, "STRING needs length\n");
            tbl.lengths[colIdx] = atoi(ptr_col->child->child->content);
        }

        

        ptr_col = ptr_col->next;
        colIdx++;
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
    buildPathToTDBtable(ptr_filepath, tbl.alias);

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