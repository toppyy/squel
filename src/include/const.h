#pragma once

/* Config for squel */

// IO
#define LINEBUFF 200
#define LF 10
#define DATAPATH "./data"
#define TDBFILEXT "tdb"

// TDB
#define TDBSCANBUFFRECORDS 10000
#define TDBMAXSTRINGSIZE 50
#define JOINTUPLESIZE    1000
#define SCANTUPLESIZE    2000

// Query execution
#define JOINBUFFSIZE    100000
#define TUPLESIZE       500    

// Define max size (in chars) of expressions and query
#define MAXQUERYSIZE 1000
#define MAXEXPRSIZE  100

#define DELIMITER   ';'
#define DELIMITERSTR ";"

// Datastructures adhere to these limits
// Strings/arrays are not reallocated if 
// they exceed the limits
#define ARRAYMAXSIZE 25
#define CHARMAXSIZE 100
