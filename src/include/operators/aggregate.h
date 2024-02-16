#pragma once
#include <stdbool.h>
#include "../buffer/buffer.h"
#include "../planner/planner.h"
#include "../executor/executor.h"

Tuple* aggregateGetTuple(Operator* op);