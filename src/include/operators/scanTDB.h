#pragma once
#include "../bufferpool/bufferpool.h"
#include "../planner/planner.h"
#include "../executor/executor.h"
#include <errno.h>

int scanTDBGetTuple(Operator* op);