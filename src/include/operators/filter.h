#pragma once
#include <stdbool.h>
#include "../bufferpool/bufferpool.h"
#include "../planner/planner.h"
#include "../executor/tuple.h"

Tuple* filterGetTuple(Operator* op);
bool evaluateTuplesAgainstFilterOps(Tuple* tpl1, Tuple* tpl2, Operator* op);