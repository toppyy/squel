#pragma once
#include <stdbool.h>
#include "../bufferpool/bufferpool.h"
#include "../planner/planner.h"

Tuple* filterGetTuple(Operator* op);
bool evaluateTupleAgainstFilterOps(Tuple* tpl, Operator* op);