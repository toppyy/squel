#include "./include/executor/executor.h"
#include "./include/parser/parser.h"
#include "./include/planner/planner.h"
#include "./include/io/tdb.h"
#include "./include/const.h"
#include "./include/squel.h"

#define METADATABUFFSIZE 10

// Globals :/
ResultSet* resultDescToPrint = NULL;
Options* OPTIONS;

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

Options* initOptions() {
    OPTIONS = malloc(sizeof(Options));
    OPTIONS->htsize = HTSIZE;
    return OPTIONS;
}

size_t getOption(Option opt) {
    switch(opt) {
        case OPT_HTSIZE:
            return OPTIONS->htsize;
    }

    printf("getOption: Tried to retrieve an unknown option\n");
    exit(1);
}

int main(int argc, char* argv[]) {

    if (argc == 1) {
        printf("Error: No query provided.\n");
        exit(1);
    }

    Options* opts = initOptions();

    size_t query_arg = 1;

    // Loop through the arguments
    for (int i = 1; i < argc; i++) {
       
        if (strcmp(argv[i], "--help") == 0) {
            printf("Help: See README.md.\n");
            return 0;
        } 
        else if (strcmp(argv[i], "--htsize") == 0) {
            i++;
            char*  endptr;
            size_t htsize = strtoull(argv[i], &endptr, 10);

            if (endptr == argv[i]) {
                printf("--htsize expects an integer\n");
                exit(1);
            }

            opts->htsize = htsize;

            query_arg += 2;
        }
    }


    if (strlen(argv[query_arg]) >= MAXQUERYSIZE) {
        printf("Error: Query length exceeds maximum.\n");
        exit(1);
    }

    /* Allocate memory for parse tree and parse the raw query */
    Node* parsetree = createParsetree();
    parse(argv[query_arg], parsetree);

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

    free(opts);

}