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
        Operator* op_filter = makeFilterOps(ON, opJoin);

        opJoin->info.join.filter = op_filter;

        return opJoin;
    }
    if (node->type == FILEPATH) {
        return makeScanOp(node);
    }
    return NULL;
}


Operator* planQuery(Node* nodeSELECT) {


    Operator* op_proj;
    
    Node* FROM = nodeSELECT->next;
    Operator* op_from = NULL;

    if (FROM->child->type == SELECT) {
        op_from = planQuery(FROM->child);
    } else {
        op_from = buildFrom(FROM->child);
    }

    Node* WHERE = nodeSELECT->next->next;

    if (WHERE != NULL) {
        
        Operator* op_filt = makeFilterOps(WHERE, op_from);
        op_proj = makeProjectOp(nodeSELECT->child, op_filt);
        
        op_proj->child  = op_filt;
        op_filt->child  = op_from;

    } else {
        op_proj = makeProjectOp(nodeSELECT->child, op_from);
        op_proj->child = op_from;
    }


    return op_proj;   
}