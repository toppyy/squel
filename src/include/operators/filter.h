#pragma once
#include <stdbool.h>
#include "../bufferpool/bufferpool.h"
#include "../planner/planner.h"
#include "../executor/tuple.h"

Tuple* filterGetTuple(Operator* op);
bool evaluateTuplesAgainstFilterOps(int poolOffset1, int poolOffset2, Operator* op);