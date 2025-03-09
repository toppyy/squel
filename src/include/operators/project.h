#pragma once
#include "../bufferpool/bufferpool.h"
#include "../planner/planner.h"
#include "../executor/tuple.h"

void projectGetTuple(Operator* op, Tuple* tpl);