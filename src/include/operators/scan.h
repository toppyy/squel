#pragma once
#include "../bufferpool/bufferpool.h"
#include "../planner/planner.h"
#include "../executor/executor.h"

Tuple* scanGetTuple(Operator* op);