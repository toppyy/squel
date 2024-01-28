#include "./include/catalog/catalog.h"


Datatype inferDatatype(char* item) {
    if (isNumeric(item[0])) {
        return DTYPE_INT;
    }
    return DTYPE_STR;
}

void catalogFile(const char* path, TableMetadata* p_tablemetadata, char delimiter) {

    if (!fileExists(path)) {
        printf("No such file '%s'.\n", path);
        exit(1);
    }

    // Read header and first line of data
    FILE* f = fopen(path, "r");
    char* header    = readLine(f);
    char* firstLine = readLine(f);
    fclose(f);

    // Parse column names from header
    size_t columnCount = 0;
    size_t cursor = 0;

    for (size_t i = 0; i < strlen(header); i++) {
        
        if (header[i] == delimiter) {
            
            columnCount++;
            cursor = 0;

            if (columnCount > COLUMNSSIZE) {
                printf("Error: column count exceeds buffer\n");
                exit(1);
            }
            continue;
        }
        p_tablemetadata->columns[columnCount].name[cursor] = header[i];
        cursor++;
    }

    p_tablemetadata->columnCount = columnCount + 1;
    free(header);
    
    // Infer datatypes
    columnCount = 0;
    cursor = 0;
    size_t firstLineLen = strlen(firstLine);
    char* itemBuffer = calloc(firstLineLen, sizeof(char));

    for (size_t i = 0; i < firstLineLen; i++) {
        
        if (firstLine[i] == delimiter) {

            p_tablemetadata->columns[columnCount].type = inferDatatype(itemBuffer);
            columnCount++;
            cursor = 0;
            memset(itemBuffer, 0, firstLineLen);
            continue;
        }
        itemBuffer[cursor] = firstLine[i];
        cursor++;
    }
    // Last column does not have delimiter after it
    p_tablemetadata->columns[columnCount].type = inferDatatype(itemBuffer);
    free(firstLine);
    free(itemBuffer);

    

};


Node*   tables[100];
size_t  tableCount = 0;


void addToTableList(Node* node) {
    tables[tableCount] = node;
    node->tableRef = tableCount;
    tableCount++;    
}

void traverse(Node* node) {

    if (node == NULL) {
        return;
    }

    if (node->type == IDENT_TBL || node->type == FILEPATH) {
        addToTableList(node);
    }

    if (node->child != NULL) {
        traverse(node->child);
    }

    if (node->next != NULL) {
        traverse(node->next);
    }
}

TableMetadata* catalogQuery(Node* astRoot, char delimiter, size_t* p_tableCount) {

    /*
        Traverse the ast and find all table references.
        If they are files, peek into the file and infer metadata.
    */

    traverse(astRoot);

    *p_tableCount = tableCount;

    TableMetadata* tbls = calloc(tableCount, sizeof(TableMetadata));

    for (size_t i = 0; i < tableCount; i++) {
        
        if (tables[i]->type == FILEPATH) {
            catalogFile(tables[i]->content, &tbls[i], delimiter);
            continue;
        }
        
        printf("Error: system catalog not implemented. Can query only files.\n");
        exit(1);
    }



    return tbls;
}
