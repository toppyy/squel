#include "../include/planner/planner.h"

void freeQueryplan(Operator *node) {

    if (node == NULL) {
        return;
    }

    if (node->child != NULL) {
        freeQueryplan(node->child);
    }


    if (node->type == OP_JOIN) {
        freeQueryplan(node->info.join.left);
        freeQueryplan(node->info.join.right);
    }

    if (node->type == OP_FILTER) {
        freeQueryplan(node->info.filter.next);
    }

    free(node);
}

void copyResultDescription(Operator* opFrom, Operator* opTo, size_t offset) {

    ResultSet resultDesc = opFrom->resultDescription;
    for (size_t i = 0; i < resultDesc.columnCount; i++) {
        opTo->resultDescription.columns[i + offset].type = resultDesc.columns[i].type;
        strcpy(opTo->resultDescription.columns[i + offset].name, resultDesc.columns[i].name);
    }
    opTo->resultDescription.columnCount += resultDesc.columnCount;

}

Operator* buildFrom(Node* node) {
    /* Node is the first child of FROM-type node */
    if (node->next != NULL && node->next->type == JOIN) {
        Operator* opJoin = (Operator*) calloc(1, sizeof(Operator));
        opJoin->info.join.left     = makeScanOp(node);
        opJoin->info.join.right    = makeScanOp(node->next->next);
        opJoin->type = OP_JOIN;
        opJoin->info.join.rightTupleCount = 0;
        opJoin->info.join.rightTupleIdx = 0;
        opJoin->info.join.rightTuplesCollected = false;

        copyResultDescription(opJoin->info.join.left, opJoin,     0);
        copyResultDescription(opJoin->info.join.right, opJoin, opJoin->resultDescription.columnCount);

        /* ON-clause */
        Node* ON = node->next->next->next;
        Operator* opFilter = makeFilterOps(ON, opJoin);

        opJoin->info.join.filter = opFilter;

        return opJoin;
    }
    if (node->type == FILEPATH) {
        return makeScanOp(node);
    }
    return NULL;
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


Operator* planQuery(Node* nodeSELECT) {


    Operator* opProj;
    
    Node* FROM = nodeSELECT->next;
    Operator* opFrom = NULL;

    if (FROM->child->type == SELECT) {
        opFrom = planQuery(FROM->child);
    } else {
        opFrom = buildFrom(FROM->child);
    }

    Node* WHERE = nodeSELECT->next->next;

    if (WHERE != NULL) {
        
        Operator* opFilter = makeFilterOps(WHERE, opFrom);
        opProj = makeProjectOp(nodeSELECT->child, opFilter);
        
        opProj->child  = opFilter;
        opFilter->child  = opFrom;

    } else {
        opProj = buildSelect(nodeSELECT->child, opFrom);
        opProj->child = opFrom;
    }


    return opProj;   
}