#include "../include/odbc/odbc.h"



SQLRETURN SQLColAttribute(	    
    SQLHSTMT          StmtHandle,  
    SQLUSMALLINT      ColumnNumber,  
    SQLUSMALLINT      FieldIdentifier,  
    SQLPOINTER        CharacterAttributePtr,  
    SQLSMALLINT       BufferLength,  
    SQLSMALLINT       *StringLengthPtr __attribute__((unused)),  
    SQLLEN            *NumericAttributePtr
)
{

    StatementHandle* handle = (StatementHandle*) StmtHandle;

    int nValue = 0;

    switch(FieldIdentifier) {
        case SQL_COLUMN_DISPLAY_SIZE:
            nValue = 20; // TODO
            break;

        case SQL_COLUMN_NAME:
        case SQL_DESC_LABEL:
        case SQL_DESC_BASE_COLUMN_NAME:
            strncpy(CharacterAttributePtr, getResultSetColumnName(handle->queryplan, ColumnNumber - 1), BufferLength);
            break;
        case SQL_DESC_TYPE:
            ctype type = getResultSetColumnType(handle->queryplan, ColumnNumber);
            switch(type) {
                case CTYPE_CHAR:
                    *NumericAttributePtr = SQL_CHAR;
                    break;
                case CTYPE_INT:
                    *NumericAttributePtr = SQL_INTEGER;
                    break;
                case CTYPE_LONG:
                    *NumericAttributePtr = SQL_BIGINT;
                    break;
                default:
                    *NumericAttributePtr = SQL_CHAR;

            }
            break;
        default:
            break;

    }

    if ( NumericAttributePtr )
		*(int*) NumericAttributePtr = nValue;

    return SQL_SUCCESS;
}