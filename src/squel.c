#include "./include/executor/executor.h"
#include "./include/parser/parser.h"
#include "./include/planner/planner.h"
#include "./include/io/tdb.h"
#include "./include/bufferpool/bufferpool.h"

#define METADATABUFFSIZE 10


ResultSet* resultDescToPrint = NULL;

void printTree(Node *node) {

    if (node == NULL) {
        return;
    }

    printf("Node (%ld) with type %d and content '%s' \n",node->identifier, node->type, node->content);

    if (node->child != NULL) {
        printf("Children: \n");
        printTree(node->child);
        printf("Children from '%s' \\w type %d done \n", node->content, node->type);
    }

    if (node->next != NULL) {
        printTree(node->next);
    }
}

void valueToChar(char* target, Tuple* tpl, size_t colOffset, Datatype type) {
    if (type == DTYPE_STR) {
        strcpy(target, tpl->data + colOffset);
        return;
    }
    if (type == DTYPE_INT) {
        char tmp[CHARMAXSIZE];
        sprintf(tmp, "%d", *(int*) (tpl->data + colOffset));
        memcpy(target, tmp, strlen(tmp));
        return;
    }
    if (type == DTYPE_LONG) {
        char tmp[CHARMAXSIZE];
        sprintf(tmp, "%ld", *(long*) (tpl->data + colOffset));
        memcpy(target, tmp, strlen(tmp));
        return;
    }
    printf("Don't know how to represent type %d as char\n", type);
    exit(1);
}




void printTuple(Tuple* tpl) {

    if (resultDescToPrint == NULL) {
        printf("No result set to print?\n");
        exit(1);
    }

    char buff[CHARMAXSIZE];

    for (size_t i = 0; i < resultDescToPrint->columnCount; i++) {
        memset(buff, 0, CHARMAXSIZE);        
        valueToChar(buff, tpl ,resultDescToPrint->pCols[i], resultDescToPrint->columns[i].type);

        if (i == 0) printf("%s",buff);
        else printf(";%s",buff);
        

    }
    printf("\n");

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

    // It's either a SELECT or a STMT
    Operator* queryplan = NULL;
    if (parsetree->next->type == SELECT) {
        /* Plan the query */
        queryplan = planQuery(parsetree->next);

        resultDescToPrint = &queryplan->resultDescription;

        /* Execute the query */
        execute(queryplan, true, printTuple);

    } else {
        executeStatement(parsetree->next);
    }

    /* Free all the memory used */
    freeTree(parsetree);
    if (queryplan != NULL) {
        freeQueryplan(queryplan);
    }

}