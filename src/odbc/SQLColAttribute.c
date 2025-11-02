#include "../include/odbc/odbc.h"

SQLRETURN SQLColAttributes(
    SQLHSTMT    hDrvStmt __attribute__((unused)),
    SQLUSMALLINT       nCol __attribute__((unused)),
    SQLUSMALLINT       nDescType __attribute__((unused)),
    SQLPOINTER	pszDesc __attribute__((unused)),
    SQLSMALLINT       nDescMax __attribute__((unused)),
    SQLSMALLINT   	*pcbDesc __attribute__((unused)),
    SQLLEN   	*pfDesc __attribute__((unused))
)
{
    printf("COLATTRIBUTES\n");
    return SQL_SUCCESS;
}

SQLRETURN SQLColumns(  
     SQLHSTMT       StatementHandle,  
     SQLCHAR *      CatalogName,  
     SQLSMALLINT    NameLength1,  
     SQLCHAR *      SchemaName,  
     SQLSMALLINT    NameLength2,  
     SQLCHAR *      TableName,  
     SQLSMALLINT    NameLength3,  
     SQLCHAR *      ColumnName,  
     SQLSMALLINT    NameLength4)
{
    printf("SQLCOLUMNS --------------------------\n");
}
                            

SQLRETURN SQLBindCol(
    	SQLHSTMT        hDrvStmt,
        SQLUSMALLINT    nCol,
        SQLSMALLINT     nTargetType,
        SQLPOINTER      pTargetValue,
        SQLLEN      nTargetValueMax,
        SQLLEN      *pnLengthOrIndicator
                        
)
{
    printf("SQLBINDCOL\n");
    return SQL_SUCCESS;
}

SQLRETURN  SQLGetDescField(  SQLHDESC            DescriptorHandle,
                             SQLSMALLINT         RecordNumber,
                             SQLSMALLINT         FieldIdentifier,
                             SQLPOINTER          Value,
                             SQLINTEGER          BufferLength,
                             SQLINTEGER         *StringLength
                          )
{

        printf("SQLBINDCOL\n");
    return SQL_SUCCESS;
}





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
    // printf("SQLColAttribute %d and field identifier %d ->", ColumnNumber, FieldIdentifier);

    StatementHandle* handle = (StatementHandle*) StmtHandle;

    int nValue = 0;


    switch(FieldIdentifier) {
        case SQL_COLUMN_DISPLAY_SIZE:
            nValue = 20; // TODO
            // printf("ok\n");
            break;


        case SQL_COLUMN_NAME:
        case SQL_DESC_LABEL:
        case SQL_DESC_BASE_COLUMN_NAME:
            strncpy(CharacterAttributePtr, getResultSetColumnName(handle->queryplan, ColumnNumber - 1), BufferLength);
            // printf("ok\n");
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
        case SQL_DESC_AUTO_UNIQUE_VALUE:
            printf("SQL_DESC_AUTO_UNIQUE_VALUE\n");
            break;

        case SQL_DESC_CONCISE_TYPE:
            printf("SQL_DESC_CONCISE_TYPE\n");
            break;

        case SQL_DESC_COUNT:
            printf("SQL_DESC_COUNT\n");
            break;



        case SQL_DESC_PRECISION:
            printf("SQL_DESC_PRECISION");
            break;
        case SQL_DESC_SCALE:
            printf("SQL_DESC_SCALE");
            break;
        case SQL_DESC_LENGTH:
            printf("SQL_DESC_LENGTH");
            break;
        case SQL_DESC_OCTET_LENGTH:
            printf("SQL_DESC_OCTET_LENGTH");
            break;
        case SQL_DESC_UNNAMED:
            printf("SQL_DESC_UNNAMED");
            break;
        case SQL_DESC_LITERAL_PREFIX:
            printf("SQL_DESC_LITERAL_PREFIX");
            break;
        case SQL_DESC_LITERAL_SUFFIX:
            printf("SQL_DESC_LITERAL_SUFFIX");
            break;
        case SQL_DESC_LOCAL_TYPE_NAME:
            printf("SQL_DESC_LOCAL_TYPE_NAME");
            break;
            
        default:
            printf("unknown %d\n", FieldIdentifier);
            break;

    }

    if ( NumericAttributePtr )
		*(int*) NumericAttributePtr = nValue;


    // ctype type = getResultSetColumnType(handle->queryplan, nCol);



    return SQL_SUCCESS;
}