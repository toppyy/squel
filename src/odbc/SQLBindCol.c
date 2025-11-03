#include "../include/odbc/odbc.h"

SQLRETURN SQLBindCol(  
      SQLHSTMT       StmtHandle,  
      SQLUSMALLINT   ColumnNumber,
      SQLSMALLINT    TargetType,  
      SQLPOINTER     TargetValuePtr,  
      SQLLEN         BufferLength,
      SQLLEN *       StrLen_or_IndPtr
)
{
    StatementHandle* handle = (StatementHandle*) StmtHandle;

    handle->bound  = true;

    // Datatype
    switch(TargetType) {

        case SQL_C_CHAR:
            handle->charBuff[ColumnNumber - 1] = TargetValuePtr;
            handle->charBuffLength[ColumnNumber - 1] = BufferLength;
            *StrLen_or_IndPtr = 4;
            break;
        case SQL_C_SBIGINT: // TODO
        case SQL_C_SLONG:
            handle->longintBuff[ColumnNumber - 1] = (long*) TargetValuePtr;
            *StrLen_or_IndPtr = sizeof(long);
            break;

        default:
            return SQL_ERROR;
    }


    return SQL_SUCCESS;
}