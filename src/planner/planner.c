#include "../include/planner/planner.h"

void freeQueryplan(Operator *node) {

    if (node == NULL) {
        return;
    }

    if (node->child != NULL) {
        freeQueryplan(node->child);
    }


    if (node->type == OP_JOIN || node->type == OP_HASHJOIN) {
        freeQueryplan(node->info.join.left);
        freeQueryplan(node->info.join.right);
        freeQueryplan(node->info.join.filter);
    }

    if (node->type == OP_FILTER) {
        freeQueryplan(node->info.filter.next);
    }

    free(node);
}


Datatype mapParsedDatatypeToEnumDatatype(char* parsed) {

    if (strcmp(parsed, "CHAR") == 0) return DTYPE_STR;
    if (strcmp(parsed, "INT") == 0)  return DTYPE_LONG;
    if (strcmp(parsed, "LONG") == 0) return DTYPE_LONG;

    printf("Don't know how map nodetype %s to datatype\n", parsed);
    exit(1);
}


void checkPtrNotNull(void* node, char* msg) {
    if (node == NULL) {
        printf("%s\n", msg);
        exit(1);
    }
}

void copyResultDescription(Operator* opFrom, Operator* opTo, size_t offset) {

    ResultSet resultDesc = opFrom->resultDescription;
    for (size_t i = 0; i < resultDesc.columnCount; i++) {
        opTo->resultDescription.columns[i + offset].type = resultDesc.columns[i].type;
        strcpy(opTo->resultDescription.columns[i + offset].name, resultDesc.columns[i].name);
        strcpy(opTo->resultDescription.columns[i + offset].resultSetAlias, resultDesc.columns[i].resultSetAlias);
    }
    opTo->resultDescription.columnCount += resultDesc.columnCount;

}

int findColIdxInResDesc(ResultSet* resultDesc, char* name, char* tblref) {

    for (size_t i = 0; i < resultDesc->columnCount; i++) {

        if (
            ( strcmp(name, resultDesc->columns[i].name) == 0 )
            &&
            ( strcmp(tblref, resultDesc->columns[i].resultSetAlias) == 0 )
        ) {
            return i;
        }
    }
    if (strlen(tblref) > 0) {
        printf("Could not find column '%s.%s' in result description\n", tblref, name);
    } else {
        printf("Could not find column '%s' in result description\n", name);
    }
    exit(1);    

}

Operator* chooseScanOp(Node* node) {
    switch(node->type) {
        case FILEPATH:
            return makeScanOp(node);
        case IDENT_TBL:
            return makeScanTDBOp(node);
        default:
            printf("Don't know how to scan node of type %d\n", node->type);
            exit(1);
    }
    return NULL;
}


Operator* buildFrom(Node* node) {
    /* Node is the first child of FROM-type node */
    if (node->next != NULL && node->next->type == JOIN) {

        Node* ON = node->next->next->next;
        // TODO: Support sub-query
        return makeJoinOp(
            chooseScanOp(node),
            chooseScanOp(node->next->next),
            ON
        );
    }

    return chooseScanOp(node);
}

Operator* buildSelect(Node* node, Operator* child) {
    /* Return a simple projection or a sequence of operators (eg. aggregation) */
    Node* tmp = node;
    bool isAggregate = false;
    while (tmp != NULL) {
        if (tmp->type == IDENT_FUN) {
            isAggregate = true;
            break;
        }
        tmp = tmp->next;
    }

    if (!isAggregate) {
        return makeProjectOp(node, child);
    }

    Operator* aggregation = makeAggregateOp(node, child);
    return aggregation;
}

Operator* buildCreateTable(Node* node) {

    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type    = OP_STMTCREATE;

    /*
        Expected tree for CREATE TABLE
            CREATE -> TABLE -> ident -> column_list
                                        -> ident
                                            -> datatype
                                        -> ...

    */

    strcpy(op->info.create.objectName, node->next->next->content);

    

    Node* ptr = node->next->next->next;

    if (ptr->type != COLUMNLIST) {
        printf("Expected a COLUMNLIST planning CREATE TABLE\n");
    }

    // Move to first column
    ptr = ptr->child;

    size_t colIndex = 0;

    while (ptr != NULL) {

        if (ptr->type != IDENT_COL) {
            printf("Expected a IDENT_COL when planning CREATE TABLE\n");
        }
        
        strcpy(op->info.create.columns[colIndex].name, ptr->content);

        Datatype type = mapParsedDatatypeToEnumDatatype(ptr->child->content);
        op->info.create.columns[colIndex].type = type;

        // For CHAR(), get the length 
        if (type == DTYPE_STR) {
            if (ptr->child->child->type != NUMBER) {
                printf("Expected a NUMBER when parsing CHAR-definition in CREATE TABLE\n");
            }
            op->info.create.columns[colIndex].size = atol(ptr->child->child->content);
        }

        colIndex++;

        ptr = ptr->next;
    }

    op->info.create.colCount = colIndex;

    return op;
}

Operator* buildExplain(Node* node) {

    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type    = OP_STMTEXPLAIN;

    op->child = planQueryAst(node->next);

    return op;

}

Operator* buildInsert(Node* node) {

    /*
        Expected tree for INSERT 
        INSERT -> ident_tbl -> SELECT
    */

    Operator* op = (Operator*) calloc(1, sizeof(Operator));
    op->type = OP_STMTINSERT;

    strcpy(op->info.insert.targetTableName, node->next->content);

    /* Plan the query */
    op->child = planQueryAst(node->next->next);

    return op;
}



Operator* planSelect(Node* nodeSELECT) {

    Operator* opProj;
    
    Node* FROM = nodeSELECT->next;
    Operator* opFrom = NULL;

    if (FROM->child->type == SELECT) {
        opFrom = planSelect(FROM->child);
    } else {
        opFrom = buildFrom(FROM->child);
    }

    Node* WHERE = nodeSELECT->next->next;

    if (WHERE != NULL) {
        
        Operator* opFilter = makeFilterOps(WHERE, opFrom);
        opProj = buildSelect(nodeSELECT->child, opFilter);
        
        opProj->child  = opFilter;
        opFilter->child  = opFrom;

    } else {
        opProj = buildSelect(nodeSELECT->child, opFrom);
        opProj->child = opFrom;
    }


    return opProj;   
}


Operator* planQueryAst(Node* node) {

    switch(node->type) {
        case STMTCREATE:
            return buildCreateTable(node);
        case SELECT:
            return planSelect(node);
        case STMTINSERT:
            return buildInsert(node);
        case STMTEXPLAIN:
            return buildExplain(node);
        default:
            printf("Internal error: don't know how to start planning from node with type of %d\n", node->type);
            exit(1);
            break;
    }
    return NULL;
}



Operator* planQuery(char* sqlStmt) {

    // Allocate memory for parse tree and parse the raw query
    Node* ast = createParsetree();
    parse(sqlStmt, ast);

    Operator* queryplan = planQueryAst((ast)->next);

    freeTree(ast);

    return queryplan;
}
