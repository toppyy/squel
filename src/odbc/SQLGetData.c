#include "../include/odbc/odbc.h"



SQLRETURN SQLGetData( SQLHSTMT      stmtHandle,
					  SQLUSMALLINT  nColumn,
					  SQLSMALLINT   nTargetType __attribute__((unused)),
					  SQLPOINTER    pTarget,
					  SQLLEN        nTargetLength __attribute__((unused)),
					  SQLLEN        *pnLengthOrIndicator
                ) {


    StatementHandle* handle = (StatementHandle*) stmtHandle;

    // It is not zero-indexed
    size_t nCol = nColumn - 1;
    ctype type = getResultSetColumnType(handle->queryplan, nCol);


    switch(nTargetType) {
        case SQL_CHAR:
            /* The target is CHAR */

            switch(type) {
                case CTYPE_LONG:
                    long lval = getLongFromIndex(handle->queryplan, handle->tpl, nCol);
                    sprintf(pTarget, "%ld", lval);
                    break;

                case CTYPE_CHAR:
                    strncpy(pTarget, getCharFromIndex(handle->queryplan, handle->tpl, nCol), nTargetLength);
                    break;

                case CTYPE_INT:
                    int ival = getIntFromIndex(handle->queryplan, handle->tpl, nCol);
                    sprintf(pTarget, "%d", ival);
                    break;
            }

            *pnLengthOrIndicator = strlen(pTarget);
            break;
        case SQL_C_LONG:
            * ((long*) pTarget) = getLongFromIndex(handle->queryplan, handle->tpl, nCol);
            *pnLengthOrIndicator = sizeof(long);
            break;
        default:
            printf("Ntargettype %d\n", nTargetType);
            break;

    }


    // switch(type) {
    //     case CTYPE_CHAR:
    //         strncpy(pTarget, getCharFromIndex(handle->queryplan, handle->tpl, nCol), nTargetLength);
    //         *pnLengthOrIndicator = strlen(pTarget);
    //         break;
    //     case CTYPE_INT:
    //         * ((int*) pTarget) = getIntFromIndex(handle->queryplan, handle->tpl, nCol);
    //         *pnLengthOrIndicator = sizeof(int);
    //         break;
    //     case CTYPE_LONG:
    //         * ((long*) pTarget) = 
    //         *pnLengthOrIndicator = sizeof(long);
    //         break;
    //     default:
    //         break;
    // }

    return SQL_SUCCESS;
}   
