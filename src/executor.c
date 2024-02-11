#include "./include/executor/executor.h"
#include "./include/buffer/buffer.h"

#define QUERYBUFFER 1000
#define SCANBUFFER  1000
#define DELIMITER   ';'
#define DELIMITERSTR ";"

char *buffercache;
char *p_buffercache;
char *bufferscan;
size_t buffercacheSize = 0;
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

        j = op->info.project.colRefs[i];

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



Tuple* filterGetTuple(Operator* op) {

    if (op == NULL) {
        printf("Passed a NULL-pointer to filterGetTuple\n");
        exit(1);
    }

    if (op->type != OP_FILTER) {
        printf("Called filterGetTuple on an operator that is not OP_FILTER\n");
    }

    if (op->child == NULL) {
        printf("OP_FILTER has no child\n");
        exit(1);
    }

    if ( op->child->getTuple == NULL) {
        printf("Child of OP_FILTER has no getTuple-method\n");
        exit(1);
    }

    Tuple* tpl = NULL;

    while (true) {
        /* Get new tuples until found something that passes the filter */

        tpl = op->child->getTuple(op->child);

        if (tpl == NULL) {
            return NULL;
        }

        
        int idx1    = op->info.filter.boolExprList[0];
        int boolOp  = op->info.filter.boolExprList[1];
        int idx2    = op->info.filter.boolExprList[2];

        enum nodeType type1 = op->info.filter.exprTypes[0];
        enum nodeType type2 = op->info.filter.exprTypes[2];


        if (
            idx1 > (int) tpl->columnCount
            ||
            idx2 > (int) tpl->columnCount 
        ) {
            printf("Filter column references out of bounds\n");
            exit(1);
        }


        bool matches = false;

        int cmpRes = 0;
        
        if (type1 == IDENT_COL && type2 == IDENT_COL)       cmpRes = strcmp(tpl->pCols[idx1],tpl->pCols[idx2]);
        else if (type1 == IDENT_COL && type2 == STRING)     cmpRes = strcmp(tpl->pCols[idx1],op->info.filter.charConstants[2]);
        else if (type1 == STRING && type2 == IDENT_COL)     cmpRes = strcmp(op->info.filter.charConstants[0], tpl->pCols[idx2]);
        else if (type1 == IDENT_COL && type2 == NUMBER) {
            int number = atoi(tpl->pCols[idx1]);
            cmpRes = number - op->info.filter.intConstants[2];
        }
        else if (type1 == NUMBER && type2 == IDENT_COL) {
            int number = atoi(tpl->pCols[idx2]);
            cmpRes = op->info.filter.intConstants[0] - number;
        } 
        

        switch(boolOp) {
            case -1:
                matches = cmpRes == 0;
                break;
            case -2:
                matches = cmpRes != 0;
                break;
            case -3:
                matches = cmpRes < 0;
                break;
            case -4:
                matches = cmpRes > 0;
                break;
            default:    
                printf("Operator %d not implemented\n", boolOp);
                exit(1);
        }

        if (matches) break;


    }

    return tpl;
}

Tuple* scanGetTuple(Operator* op) {

    if (op == NULL) {
        printf("NULL pointer passed to scanGetTuple\n");
        exit(1);
    }

    // Read header and first line of data
    FILE* f = op->info.scan.tablefile;
    if (f == NULL) {
        f = fopen(op->info.scan.table.path, "r");
        readLineToBuffer(f, bufferscan, SCANBUFFER); // Read header
        op->info.scan.tablefile = f;
    
    }

    char* buffercacheWithCursor = buffercache;
    char* line = readLineToBuffer(f, buffercacheWithCursor, QUERYBUFFER - buffercacheSize);

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
    buffercache += len + 1;
    buffercacheSize += len + 1;

    return &tplbuffer.tuples[idx];
}

Tuple* concat_tuples(Tuple* left, Tuple* right) {

    if (
        left == NULL ||
        right == NULL
    ) {
        printf("Passed a NULL pointer to concat_tuples\n");
        exit(1);
    }

    tplbuffer.tupleCount++;
    size_t idx = tplbuffer.tupleCount;
    size_t len = left->size + right->size;

    tplbuffer.tuples[idx].columnCount = left->columnCount + right->columnCount;
    tplbuffer.tuples[idx].size        = len;

    // Fill identifiers for tuple
    for (size_t i = 0; i < tplbuffer.tuples[idx].columnCount; i++) {
        tplbuffer.tuples[idx].identifiers[i] = -1; // TODO: Any use?
    }
    // Add pointers to start of each column
    // Reuse the data in the original tuples
    // Move only pointers to two columns; do not copy data


    size_t i = 0;
    for (size_t j = 0; j < left->columnCount; j++) {
        tplbuffer.tuples[idx].pCols[i] = left->pCols[j];
        i++;
    }

    for (size_t j = 0; j < right->columnCount; j++) {
        tplbuffer.tuples[idx].pCols[i] = right->pCols[j];
        i++;
    }

    return &tplbuffer.tuples[idx];
}

Tuple* joinGetTuple(Operator* op) {

    if (
        op->info.join.left == NULL ||
        op->info.join.right == NULL
        ) {
        printf("Join left or right operator is NULL\n");
        exit(1);
    }
    

    if (
        op->info.join.left->type != OP_SCAN ||
        op->info.join.right->type != OP_SCAN
        ) {
        printf("Join left or right operator not of type OP_SCAN\n");
        printf("Left type: %d. Right type: %d.\n", op->info.join.left->type, op->info.join.right->type );
        exit(1);
    }

    Tuple* right_tuple = NULL;
    
    if (op->info.join.rightTuplesCollected) {
        if (op->info.join.rightTupleIdx >= op->info.join.rightTupleCount) {
            op->info.join.rightTupleIdx = 0;
            op->info.join.last_tuple = NULL;
        }
        right_tuple = op->info.join.rightTuples[op->info.join.rightTupleIdx++];
        
    } else {
        right_tuple = op->info.join.right->getTuple(op->info.join.right);

        if (right_tuple == NULL) {
            op->info.join.rightTuplesCollected = true;
            op->info.join.last_tuple = NULL;
            op->info.join.rightTupleIdx = 0;
            right_tuple = op->info.join.rightTuples[op->info.join.rightTupleIdx++];
            
            
        } else {
            op->info.join.rightTuples[op->info.join.rightTupleIdx++] = right_tuple;
            op->info.join.rightTupleCount++;

        }
    }
    if (op->info.join.last_tuple == NULL) {
        op->info.join.last_tuple   = op->info.join.left->getTuple(op->info.join.left);
        if (op->info.join.last_tuple == NULL) {
            return NULL;
        }
    }

    return concat_tuples(op->info.join.last_tuple , right_tuple);
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
        case (OP_FILTER):
            op->getTuple = &filterGetTuple;
            break;
        case (OP_JOIN):
            op->getTuple = &joinGetTuple;
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


void doAssignGetTupleFunction(Operator* p_op) {

    if (p_op == NULL) {
        return;
    }

    assignGetTupleFunction(p_op);

    if (p_op->child != NULL) {
        doAssignGetTupleFunction(p_op->child);
    }

    if (p_op->type == OP_JOIN) {
        doAssignGetTupleFunction(p_op->info.join.left);
        doAssignGetTupleFunction(p_op->info.join.right);
    }
}

void printBuffercache() {
    size_t i = 0;
    char prev = 'a', cur = 'a';
    while (i < QUERYBUFFER) {
        prev = cur;   
        cur = p_buffercache[i++];
        if (prev == '\0' && cur == '\0') {
            continue;
        }
        if (cur == '\0') {
            printf("\n");
            continue;
        }
        printf("%c",cur);
    }
}

void execute(Operator *op) {

    p_buffercache = calloc(QUERYBUFFER, sizeof(char)); 
    bufferscan  = calloc(SCANBUFFER, sizeof(char)); 
    buffercache = p_buffercache;

    tplbuffer.tupleCount = 0;

    doAssignGetTupleFunction(op);

    struct Tuple* tpl;


    for (;;) {
        tpl = op->getTuple(op);
        if (tpl == NULL) break;

        printTuple(tpl);
    };

    free(p_buffercache);
    free(bufferscan);
}
