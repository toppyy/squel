#include "./include/executor/executor.h"
#include "./include/catalog/catalog.h"
#include "./include/parser/parser.h"
#include "./include/binder/binder.h"
#include "./include/planner/planner.h"


#define METADATABUFFSIZE 10


void printTree(Node *node) {

    if (node == NULL) {
        return;
    }

    printf("Node (%ld) with type %d and content '%s' from tbl %d and col %d\n",node->identifier, node->type, node->content, node->tableRef, node->colRef);

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
        printf("Error: No query provided.\n");
        exit(1);
    }

    if (strlen(argv[1]) >= MAXQUERYSIZE) {
        printf("Error: Query length exceeds maximum.\n");
        exit(1);
    }

    /* Allocate memory for parse tree and parse the raw query */
    Node* parsetree = createParsetree();
    parse(argv[1], parsetree);

    /* Catalog the query */
    char delimiter = ';';
    size_t tableCount = 0;
    TableMetadata* tables = catalogQuery(parsetree, delimiter, &tableCount);
    
    /* Annotate the abstract syntax tree */
    bind(parsetree, tables, tableCount);

    // printTree(parsetree);

    /* Plan the query */
    Operator* queryplan = planQuery(parsetree, tables, tableCount);

    /* Execute the query */
    execute(queryplan);


    /* Free all the memory used */
    freeTree(parsetree);
    if (queryplan != NULL) {
        freeQueryplan(queryplan);
    }
    free(tables);

}