#pragma once
#include "../bufferpool/bufferpool.h"
#include "../planner/planner.h"
#include "../executor/executor.h"
#include "../executor/tuple.h"
#include <errno.h>

Tuple* scanTDBGetTuple(Operator* op);