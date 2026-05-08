#include "./include/squel.h"

void tupleColValueToChar(char* target, Tuple* tpl, size_t colIdx, Datatype type) {

    
    if (type == DTYPE_STR) {
        strcpy(target, getTupleColByIndex(tpl, colIdx));
        return;
    }
    if (type == DTYPE_INT) {
        char tmp[CHARMAXSIZE];
        sprintf(tmp, "%d", *(int*) source);
        memcpy(target, tmp, strlen(tmp));
        return;
    }
    if (type == DTYPE_LONG) {
        char tmp[CHARMAXSIZE];
        sprintf(tmp, "%ld", getTupleLongColByIndex(tpl, colIdx));
        memcpy(target, tmp, strlen(tmp));
        return;
    }
    printf("Don't know how to represent type %d as char\n", type);
    exit(1);
}

void printTuple(Operator* op, Tuple* tpl) {

    if (op == NULL) {
        printf("No result set to print?\n");
        exit(1);
    }

    char buff[CHARMAXSIZE];


    char firstPrinted = 0;

    for (size_t i = 0; i < op->resultDescription.columnOrderCount; i++) {
        
        size_t colIdx = op->resultDescription.columnOrder[i];

        if (!op->resultDescription.columns[colIdx].active) continue;

        memset(buff, 0, CHARMAXSIZE);

        Datatype typeToPrint = DTYPE_STR;
        
        if (isCasted(tpl,colIdx)) typeToPrint = op->resultDescription.columns[colIdx].type;
        
        tupleColValueToChar(buff, tpl, colIdx, typeToPrint);

        if (firstPrinted == 0) printf("%s",buff);
        else printf(";%s",buff);

        firstPrinted = 1;

    }

    printf("\n");

}



void printColnames(Operator* queryplan) {

    if (queryplan == NULL) return;
    if (queryplan->resultDescription.columnCount == 0) return;

    char firstPrinted = 0;

    for (size_t i = 0; i < queryplan->resultDescription.columnOrderCount; i++) {
        
        size_t colIdx = queryplan->resultDescription.columnOrder[i];

        if (!queryplan->resultDescription.columns[colIdx].active) continue;

        if (firstPrinted == 0)    
           printf("%s", queryplan->resultDescription.columns[colIdx].name);

        if (firstPrinted > 0)
           printf("%c%s", DELIMITER, queryplan->resultDescription.columns[colIdx].name);
        
        firstPrinted = 1;
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

    printColnames(queryplan);
    execute(queryplan, printTuple);

    /* Free all the memory used */
    // freeTree(parsetree);

    if (queryplan != NULL) {
        freeQueryplan(queryplan);
    }

    
    free(opts);


}   