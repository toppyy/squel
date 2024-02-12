#include "./include/executor/executor.h"
#include "./include/parser/parser.h"
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
        printf("Children from '%s' \\w type %d done \n", node->content, node->type);
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

    // printTree(parsetree);

    /* Plan the query */
    Operator* queryplan = planQuery(parsetree->next);

    /* Execute the query */
    execute(queryplan);

    /* Free all the memory used */
    freeTree(parsetree);
    if (queryplan != NULL) {
        freeQueryplan(queryplan);
    }

}