#pragma once
#include "../parser/parsetree.h"
#include "../catalog/catalog.h"

/*
    The purpose of the binder is to annotate the parse tree. By annotating I mean
        - matching identifiers in the raw query to columns in the data 
        - ensuring functions called exists
        - expanding the '*' operator to refer to all columns in the query

*/


void bind(Node* astRoot, TableMetadata* tables, size_t tableCount);