#include "../include/planner/planner.h"



Datatype inferDatatype(char* item) {
    
    size_t i = 0;
    while (item[i] != '\0') {
        if (!isNumeric(item[i++])) return DTYPE_STR;
    }
    return DTYPE_INT;
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

    // Store the path
    strcpy(p_tablemetadata->path, path);

    // Parse column names from header
    size_t columnCount = 0;
    size_t cursor = 0;

    for (size_t i = 0; i < strlen(header); i++) {
        
        if (header[i] == delimiter) {
            
            columnCount++;
            p_tablemetadata->columns[columnCount].name[cursor + 1] = '\0';
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

