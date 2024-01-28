#include "./include/catalog/catalog.h"
#include "./include/parser/parser.h"
#include "./include/binder/binder.h"

#define METADATABUFFSIZE 10


void printTree(Node *node) {

    if (node == NULL) {
        return;
    }

    printf("Node with type %d and content '%s' from tbl %d and col %d\n", node->type, node->content, node->tableRef, node->colRef);

    if (node->child != NULL) {
        printf("Children: \n");
        printTree(node->child);
        printf("Children from type %d done \n", node->type);
    }

    if (node->next != NULL) {
        printTree(node->next);
    }
}

int main(int argc, char* argv[]) {

    if (argc == 1) {
        printf("Need that SQL!\n");
        exit(1);
    }

    if (strlen(argv[1]) >= MAXQUERYSIZE) {
        printf("Error: Query length exceeds maximum.\n");
        exit(1);
    }

    /* Allocate memory for parse tree and parse the raw query */
    Node* ast = createParsetree();
    parse(argv[1], ast);

    /* Catalog the query */
    char delimiter = ';';
    size_t tableCount = 0;
    TableMetadata* tables = catalogQuery(ast, delimiter, &tableCount);
    
    /* Annotate the abstract syntax tree */
    bind(ast, tables, tableCount);

    /* Free all the memory used */
    freeTree(ast);
    free(tables);

}