#include "../include/operators/aggregate.h"

long count(long result, long num __attribute__((unused))) {
    return result + 1;
}

long max(long result, long num) {
    return num > result ? num : result;
}

long sum(long result, long num) {
    return num + result;
}

long min(long result, long num) {
    return num < result ? num : result;
}



void aggregateGetTuple(Operator* op, Tuple* tpl) {
    
    checkPtrNotNull(op->child, "OP_AGGREGATE has no child.");
    checkPtrNotNull(op->child->getTuple, "Child of OP_AGGREGATE has no getTuple-method.");

    if (op->info.aggregate.aggregationDone) {
        markTupleAsEmpty(tpl);
        return;
    }

    // TODO:
    //     if (opToIterate->resultDescription.columns[colIdx].type != DTYPE_LONG) {
    //     printf("Sum not implement for other datatypes besides long\n");
    //     exit(1);
    // }


    size_t colOffset = op->child->resultDescription.colrefs[op->info.aggregate.colToAggregate];


    long (*agg_fun)(long result, long num);
    long result = 0, colNumber = 0;


    switch(op->info.aggregate.aggtype) {
        case COUNT:
            agg_fun = count;
            break;
        case SUM:
            agg_fun = sum;
            break;
        case AVG:
            agg_fun = sum; // See below why
            break;
        case MAX:
            agg_fun = max;
            break;
        case MIN:
            agg_fun = min;
            result = __LONG_MAX__;
            break;
        default:
            printf("Aggregation type (%d) not implemented\n", op->info.aggregate.aggtype);
            exit(1);
    }


    size_t observations = 0;
    
    Tuple* tmpTpl = initTupleOfSize(TUPLESIZE);

    for (;;) {

        op->child->getTuple(op->child, tmpTpl);
        
        if (isTupleEmpty(tmpTpl)) {
            break;
        }

        observations++;

        if (op->info.aggregate.aggtype != COUNT) {
            colNumber = getTupleLongColByIndex(tmpTpl, colOffset);
        }

        result = agg_fun(result, colNumber);
    };

    freeTuple(tmpTpl);


    if (op->info.aggregate.aggtype == AVG) {
        result = result / observations;
    }


    op->resultDescription.columnCount = 1;
    op->resultDescription.colrefs[0] = 0;
    op->info.aggregate.aggregationDone = true;

    
    *(long*)(tpl->data) = result;
    tpl->longs[0]   = result;
    tpl->longCount  = 1;
    tpl->casted[0]  = 1;
    tpl->offsets[0] = 0;


}
