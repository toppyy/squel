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

Operator* buildFrom(Node* node) {
    /* Node is the first child of FROM-type node */
    if (node->next != NULL && node->next->type == JOIN) {
        Operator* op_join = (Operator*) calloc(1, sizeof(Operator));
        op_join->info.join.left     = makeScanOp(node);
        op_join->info.join.right    = makeScanOp(node->next->next);
        op_join->type = OP_JOIN;
        op_join->info.join.rightTupleCount = 0;
        op_join->info.join.rightTupleIdx = 0;
        op_join->info.join.rightTuplesCollected = false;


        ResultSet result_desc = op_join->info.join.left->resultDescription;
        for (size_t i = 0; i < result_desc.columnCount; i++) {
            op_join->resultDescription.columns[i].type = result_desc.columns[i].type;
            strcpy(op_join->resultDescription.columns[i].name, result_desc.columns[i].name);
        }
        op_join->resultDescription.columnCount = result_desc.columnCount;

        result_desc = op_join->info.join.right->resultDescription;
        size_t rd_i = op_join->resultDescription.columnCount;
        for (size_t i = 0; i < result_desc.columnCount; i++) {
            op_join->resultDescription.columns[i + rd_i].type = result_desc.columns[i].type;
            strcpy(op_join->resultDescription.columns[i + rd_i].name, result_desc.columns[i].name);
        }
        op_join->resultDescription.columnCount += result_desc.columnCount;

        /* ON-clause */
        Node* ON = node->next->next->next;
        Operator* op_filter = makeFilterOps(ON, op_join);

        op_join->info.join.filter = op_filter;

        return op_join;
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