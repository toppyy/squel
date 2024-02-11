#pragma once
#include "../buffer/buffer.h"
#include "../planner/planner.h"

Tuple* filterGetTuple(Operator* op);
bool evaluateTupleAgainsFilterOp(Tuple* tpl, Operator* op);