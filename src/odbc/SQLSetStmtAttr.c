#include "../include/odbc/odbc.h"


SQLRETURN SQLSetStmtAttr(  
     SQLHSTMT      StmttHandle __attribute__((unused)),
     SQLINTEGER    Attribute,  
     SQLPOINTER    ValuePtr __attribute__((unused)),
     SQLINTEGER    StringLength __attribute__((unused))
)
{

    switch(Attribute) {
        case SQL_ATTR_ROWS_FETCHED_PTR:
            // We'll always return 1, so why not just write it immediately
            *((int*) ValuePtr) = 1;
            break;

    }
    return SQL_SUCCESS;
}