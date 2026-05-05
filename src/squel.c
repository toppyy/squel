#include "./include/squel.h"


// Globals :/
ResultSet* resultDescToPrint = NULL;



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

#include <stdint.h>
void dump_bytes(const void *buf, size_t len) {
    const uint8_t *b = (const uint8_t*) buf;
    for (size_t i = 0; i < len; ++i) {
        if (i % 16 == 0) printf("%08zx: ", i);
        printf("%02x ", b[i]);
        if ((i % 16) == 15 || i == len - 1) printf("\n");
}}


void printTuple(Operator* op, Tuple* tpl) {

    if (op == NULL) {
        printf("No result set to print?\n");
        exit(1);
    }

    char buff[CHARMAXSIZE];

    // dump_bytes(tpl->data, tpl->size);

    // for (size_t i = 0; i < tpl->size;i++) printf("%c", ((char*) (tpl->data))[i]  );
    // printf("\n");

    // printf("---\n");
    // printf("queryplan->resultDescription.id: %ld\n", resultDescToPrint->id);
    // for (size_t i = 0; i < resultDescToPrint->columnCount; i++) printf("col %ld: %d\n", i, resultDescToPrint->columns[i].type);
    // printf("---\n");

    for (size_t i = 0; i < op->resultDescription.columnCount; i++) {
        memset(buff, 0, CHARMAXSIZE);

        Datatype typeToPrint = DTYPE_STR;

        size_t colIdx = op->resultDescription.colrefs[i];
        
        if (tpl->casted[colIdx]) typeToPrint = op->resultDescription.columns[i].type;
        
        // printf("colIdx %ld (casted: %c, type: %d): '", colIdx, tpl->casted[colIdx] ? 'T' : 'F', typeToPrint);

        tupleColValueToChar(buff, tpl, colIdx, typeToPrint);

        // printf("%s'\n", buff);

        if (i == 0) printf("%s",buff);
        else printf(";%s",buff);



    }

    // for (size_t i = 0; i < tpl->longCount; i++) printf("long %ld: %ld\n", i, tpl->longs[i]);

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