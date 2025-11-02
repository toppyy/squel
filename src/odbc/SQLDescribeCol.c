#include "../include/odbc/odbc.h"

SQLRETURN SQLDescribeCol( SQLHSTMT    	hDrvStmt __attribute__((unused)),
						  SQLUSMALLINT	nCol __attribute__((unused)),
						  SQLCHAR     	*szColName __attribute__((unused)),
						  SQLSMALLINT 	nColNameMax __attribute__((unused)),
						  SQLSMALLINT 	*pnColNameLength __attribute__((unused)),
						  SQLSMALLINT 	*pnSQLDataType __attribute__((unused)),
						  SQLULEN 	    *pnColSize __attribute__((unused)),
						  SQLSMALLINT 	*pnDecDigits __attribute__((unused)),
						  SQLSMALLINT 	*pnNullable __attribute__((unused))
) {

    printf("SQLDSCRI------------\n");

    return SQL_ERROR;

}