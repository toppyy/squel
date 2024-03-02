#pragma once
#include <stdbool.h>
#include "../bufferpool/bufferpool.h"
#include "../planner/planner.h"

int filterGetTuple(Operator* op);
bool evaluateTupleAgainstFilterOps(int poolOffset, Operator* op);