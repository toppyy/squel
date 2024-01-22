#include "./include/utils.h"
#include "./include/parser.h"

int main(int argc, char* argv[]) {

    if (argc == 1) {
        printf("Need that SQL!\n");
        exit(1);
    }

    if (strlen(argv[1]) >= MAXQUERYSIZE) {
        printf("Query length exceeds maximum :/\n");
        exit(1);
    }

    strcpy(rawSql,argv[1]);
    qsize = strlen(rawSql);
    getNextChar();
    query();
}