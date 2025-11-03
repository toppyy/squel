#include "../include/odbc/odbc.h"

SQLRETURN SQLGetInfo(  
     SQLHDBC         ConnectionHandle __attribute__((unused)),
     SQLUSMALLINT    InfoType,
     SQLPOINTER      InfoValuePtr,
     SQLSMALLINT     BufferLength,  
     SQLSMALLINT *   StringLengthPtr
)
{
    switch(InfoType) {
        case SQL_DBMS_NAME:
        case SQL_DATA_SOURCE_NAME:
        case SQL_DATABASE_NAME:
            if (BufferLength >= 6) {
                strncpy(InfoValuePtr, "squel", 6);
                *StringLengthPtr = 6;
            }
            break;
        case SQL_DRIVER_ODBC_VER:
            *(int*) InfoValuePtr = 0x300;
            break;
        case SQL_DBMS_VER:
        case SQL_DRIVER_VER:
            *(int*) InfoValuePtr = 1;
            break;
        default:
            break;
            
    }

    return SQL_SUCCESS;
}