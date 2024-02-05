#include "./include/executor/executor.h"
#include "./include/buffer/buffer.h"

#define QUERYBUFFER 10 * 1000
#define SCANBUFFER  500
#define DELIMITER   ';'
#define DELIMITERSTR ";"

char *buffercache;
char *bufferscan;
TupleBuffer tplbuffer;


Tuple* projectGetTuple(Operator* op) {

    if (op->child == NULL) {
        printf("OP_PROJECT has no child\n");
        exit(1);
    }

    if ( op->child->getTuple == NULL) {
        printf("Child of OP_PROJECT has no getTuple-method\n");
        exit(1);
    }
    
    Tuple* tpl = op->child->getTuple(op->child);

    if (tpl == NULL) {
        return NULL;
    }

    
    size_t  newcolumnCount = 0;
    size_t  newidentifiers[TUPLECOLS];
    size_t  newSize = 0;
    char*   newpCols[TUPLECOLS];
    
    size_t  j = 0;

    for (size_t i = 0; i < op->resultDescription.columnCount; i++) {

        bool included = false;
        for (j = 0; j < tpl->columnCount; j++) {
            
            
            included = op->resultDescription.columns[i].identifier == tpl->identifiers[j];
            if (included) break;
        }

        if (!included) continue;


        newSize += strlen(tpl->pCols[j]) + 1;
        
        newidentifiers[newcolumnCount] = tpl->identifiers[j];
        newpCols[newcolumnCount] = tpl->pCols[j];
        newcolumnCount++;

    }

    for (size_t i = 0; i < newcolumnCount; i++) {
        tpl->identifiers[i] = newidentifiers[i];
        tpl->pCols[i] = newpCols[i];
    }

    tpl->columnCount = newcolumnCount;
    tpl->size = newSize;

    return tpl;

}

Tuple* scanGetTuple(Operator* op) {


    // Read header and first line of data
    FILE* f = op->info.scan.tablefile;
    if (f == NULL) {
        f = fopen(op->info.scan.table.path, "r");
        readLineToBuffer(f, bufferscan, SCANBUFFER); // Read header
        op->info.scan.tablefile = f;
    
    }

    char* buffercacheWithCursor = buffercache + op->info.scan.cursor;
    char* line = readLineToBuffer(f, buffercacheWithCursor, SCANBUFFER - op->info.scan.cursor);

    if (line == NULL) {
        
        fclose(op->info.scan.tablefile);
        return NULL;

    }

    tplbuffer.tupleCount++;
    size_t idx = tplbuffer.tupleCount;
    size_t len = strlen(line);

    tplbuffer.tuples[idx].columnCount = op->resultDescription.columnCount;
    tplbuffer.tuples[idx].size        = len;

    // Fill identifiers for tuple
    for (size_t i = 0; i < op->resultDescription.columnCount; i++) {
        tplbuffer.tuples[idx].identifiers[i] = op->resultDescription.columns[i].identifier;
    }
    // Add pointers to start of each column
    char* dlmtr = NULL;

    size_t cursor = 0;

    tplbuffer.tuples[idx].pCols[0] = buffercacheWithCursor;
    size_t i = 1;

    for (;;) {
        
        dlmtr = strchr(buffercacheWithCursor + cursor, DELIMITER);
        
        if (dlmtr == NULL) {
            tplbuffer.tuples[idx].pCols[i] =  buffercacheWithCursor;
            break;
        }
        cursor += dlmtr - (buffercacheWithCursor + cursor) + 1;
        
        tplbuffer.tuples[idx].pCols[i] =  buffercacheWithCursor + cursor;
        
        (*dlmtr) = '\0'; // Replace delimiter with NULL so each column is a NULL-terminated string
        i++;
    
    };
    
    op->info.scan.cursor += len;
    return &tplbuffer.tuples[idx];
}


void assignGetTupleFunction(Operator *op) {

    if (op == NULL) {
        printf("Passed a null pointer to assignGetTupleFunction\n");
        exit(1);
    }
    switch(op->type) {
        case(OP_SCAN):
            op->getTuple = &scanGetTuple;
            break;
        case(OP_PROJECT):
            op->getTuple = &projectGetTuple;
            break;
        default:
            printf("Don't know how to handle op-type %d\n", op->type);
            exit(1);
    }
}


void printTuple(Tuple* tpl) {

    char* printBuff = calloc(tpl->size, sizeof(char));

    for (size_t i = 0; i < tpl->columnCount; i++) {
        strcpy(printBuff + strlen(printBuff), tpl->pCols[i]);
        if (i == tpl->columnCount - 1) continue;
        strcpy(printBuff + strlen(printBuff), DELIMITERSTR);
    }

    printf("%s\n", printBuff);

    free(printBuff);
}


void execute(Operator *op) {

    buffercache = calloc(QUERYBUFFER, sizeof(char)); 
    bufferscan  = calloc(SCANBUFFER, sizeof(char)); 

    tplbuffer.tupleCount = 0;

    assignGetTupleFunction(op);
    assignGetTupleFunction(op->child);

    struct Tuple* tpl;

    for (;;) {
        tpl = op->getTuple(op);
        if (tpl == NULL) break;

        printTuple(tpl);
    };

    free(buffercache);
    free(bufferscan);
}
