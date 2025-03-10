#pragma once
#include <stdbool.h>
#include "../planner/planner.h"
#include "../executor/tuple.h"

void filterGetTuple(Operator* op, Tuple* tpl);
bool evaluateTuplesAgainstFilterOps(Tuple* tpl1, Tuple* tpl2, Operator* op);