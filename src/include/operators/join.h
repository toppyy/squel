#pragma once
#include "../bufferpool/bufferpool.h"
#include "../planner/planner.h"
#include "../executor/executor.h"

Tuple* joinGetTuple(Operator* op);