#include "../include/odbc/odbc.h"

RETCODE SQL_API _Fetch(SQLHSTMT stmthandle) {

    StatementHandle* handle = (StatementHandle*) stmthandle;

    fetchTuple(handle->queryplan, handle->tpl);

    if (isTupleEmpty(handle->tpl)) return SQL_NO_DATA;

    handle->rowcount++;

    if (handle->bound) {

        for (size_t colIdx = 0; colIdx < getResultSetColumnCount(handle->queryplan); colIdx++) {
            
            ctype type = getResultSetColumnType(handle->queryplan, colIdx);


            switch(type) {
                case CTYPE_LONG:
                    *handle->longintBuff[colIdx] = getLongFromIndex(handle->queryplan, handle->tpl, colIdx);
                    break;

                case CTYPE_CHAR:
                    if (handle->charBuff != NULL) {
                        const char* data = getCharFromIndex(handle->queryplan, handle->tpl, colIdx);
                        memset(handle->charBuff[colIdx], 0, handle->charBuffLength[colIdx]);
                        memcpy(handle->charBuff[colIdx], data, strlen(data));
                    }
                    break;

                case CTYPE_INT:
                    *handle->longintBuff[colIdx] = getIntFromIndex(handle->queryplan, handle->tpl, colIdx);
                    break;

                default:
                    return SQL_ERROR;
            }
        }
    }


    return SQL_SUCCESS;
}

