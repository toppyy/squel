#include "./include/squel.h"


// Globals :/
ResultSet* resultDescToPrint = NULL;



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



void printColnames(Operator* queryplan) {

    if (queryplan == NULL) return;
    if (queryplan->resultDescription.columnCount == 0) return;

    printf("%s", queryplan->resultDescription.columns[0].name);
    for (size_t i = 1; i < queryplan->resultDescription.columnCount; i++) {
        printf("%c%s", DELIMITER, queryplan->resultDescription.columns[i].name);
    }
    printf("\n");

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

        else if (strcmp(argv[i], "--hashjoin") == 0) {
            i++;
            char*  endptr;
            size_t enableHashjoin = strtoull(argv[i], &endptr, 10);

            if (endptr == argv[i]) {
                printf("--hashjoin expects an integer\n");
                exit(1);
            }

            opts->enableHashjoin = enableHashjoin;

            query_arg += 2;
        }
    }


    if (strlen(argv[query_arg]) >= MAXQUERYSIZE) {
        printf("Error: Query length exceeds maximum.\n");
        exit(1);
    }

    Operator* queryplan = planQuery(argv[query_arg]);

    resultDescToPrint = &queryplan->resultDescription;

    printColnames(queryplan);
    execute(queryplan, printTuple);

    /* Free all the memory used */
    // freeTree(parsetree);

    if (queryplan != NULL) {
        freeQueryplan(queryplan);
    }

    
    free(opts);


}   