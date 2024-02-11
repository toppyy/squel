#pragma once
#include "../buffer/buffer.h"
#include "../planner/planner.h"
#include "../executor/executor.h"

Tuple* joinGetTuple(Operator* op);