#include "./include/binder/binder.h"


void bindColumn(Node* node,TableMetadata* tables, size_t tableCount) {

    char* colName = node->content;

    for (size_t i = 0; i < tableCount; i++) {
        for (size_t ci = 0; ci < tables[i].columnCount; ci++) {

            if (strcmp(colName,tables[i].columns[ci].name) != 0) continue;

            if (node->colRef >= 0) {
                // Already matched, ambiguous reference
                printf("Error: Ambiguous reference '%s'.\n", colName);
                exit(1);
            }

            node->tableRef  = i;
            node->colRef    = ci;
        }
    }
}




void traverseForBinding(Node* node, TableMetadata* tables, size_t tableCount) {

    if (node == NULL) {
        return;
    }

    if (node->type == IDENT_COL) {
        bindColumn(node, tables, tableCount);
    }

    if (node->child != NULL) {
        traverseForBinding(node->child, tables, tableCount);
    }

    if (node->next != NULL) {
        traverseForBinding(node->next, tables, tableCount);
    }
}


void bind(Node* astRoot, TableMetadata* tables, size_t tableCount) {

    traverseForBinding(astRoot,tables, tableCount);

}