#include "../include/odbc/odbc.h"


#define _min(a,b) ((a < b)?a:b)

SQLRETURN SQLDescribeCol(  
      SQLHSTMT       StmtHandle,  
      SQLUSMALLINT   ColumnNumber,  
      SQLCHAR *      ColumnName,  
      SQLSMALLINT    BufferLength,
      SQLSMALLINT *  NameLengthPtr,  
      SQLSMALLINT *  DataTypePtr,  
      SQLULEN *      ColumnSizePtr,  
      SQLSMALLINT *  DecimalDigitsPtr,  
      SQLSMALLINT *  NullablePtr
)
{

    StatementHandle* handle = (StatementHandle*) StmtHandle;

    SQLUSMALLINT colIdx = ColumnNumber - 1;

    // Name
    const char* colname = getResultSetColumnName(handle->queryplan, colIdx);
    memset(ColumnName, 0, BufferLength);
    memcpy(ColumnName,  colname, _min(BufferLength,(SQLSMALLINT) strlen(colname))  );
    *NameLengthPtr = strlen(colname);

    // Datatype
    ctype type = getResultSetColumnType(handle->queryplan, colIdx);
    switch(type) {
        case CTYPE_CHAR:
            *DataTypePtr = SQL_CHAR;
            break;
        case CTYPE_INT:
            *DataTypePtr = SQL_INTEGER;
            break;
        case CTYPE_LONG:
            *DataTypePtr = SQL_BIGINT;
            break;
        default:
            *DataTypePtr = SQL_CHAR;

    }

    *ColumnSizePtr = 10; // TODO
    *DecimalDigitsPtr = 0;
    *NullablePtr = SQL_NULLABLE;

    return SQL_SUCCESS;
}
