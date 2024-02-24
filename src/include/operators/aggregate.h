#pragma once
#include <stdbool.h>
#include "../bufferpool/bufferpool.h"
#include "../planner/planner.h"
#include "../executor/executor.h"

Tuple* aggregateGetTuple(Operator* op);