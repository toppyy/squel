#pragma once
#include "../bufferpool/bufferpool.h"
#include "../planner/planner.h"
#include "../executor/tuple.h"

Tuple* projectGetTuple(Operator* op);