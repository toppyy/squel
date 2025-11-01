#include "../include/api/api.h"

size_t getResultSetColumnCount(Operator* queryplan) {
    return queryplan->resultDescription.columnCount;
}


const char* getResultSetColumnName(Operator* queryplan, size_t colIdx) {
    return (const char*) queryplan->resultDescription.columns[colIdx].name;
}




ctype getResultSetColumnType(Operator* queryplan, size_t colIdx)  {
    /* Use ctype to abstract away internal enum */
    switch(queryplan->resultDescription.columns[colIdx].type) {
        case DTYPE_STR:
            return CTYPE_CHAR;
        case DTYPE_INT:
            return CTYPE_INT;
        case DTYPE_LONG:
            return CTYPE_LONG;
        default:
            printf("api-error: Unknown datatype!\n");
            exit(1);
    }
}


