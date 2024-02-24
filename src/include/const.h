#pragma once

/* Config for squel */

// IO
#define LINEBUFF 200
#define LF 10

// Bufferpool
#define QUERYBUFFER 10000
#define TUPLEDATAMAXSIZE 500

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