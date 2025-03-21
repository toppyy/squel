
BUILDIR = build

SRC=./src/
ODIR=build

__OBJ = $(wildcard -r src/*.c) $(wildcard -r src/**/*.c) $(wildcard -r src/**/**/*.c)
_OBJ = $(patsubst %.c, %.o, $(__OBJ))
OBJ = $(patsubst src/%, $(ODIR)/%, $(_OBJ))


CC = gcc
CFLAGS = -Wall -Wextra -Werror

# Define a target to compile all source files
all: dirs squel

squel: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(ODIR)/$@

$(ODIR)/%.o: $(SRC)%.c
	$(CC) -g -c $< -o $@  $(CFLAGS)

dirs:
	mkdir -p data build/parser build/planner/operators build/binder build/io build/executor build/executor/statements build/operators build/util/hashmap

clean:
	rm -f ./build/squel $(OBJ)

# Run tests
tests:
	./test/bats-core/bin/bats ./test

perf-prep:
	. ./perf/prep_perf.sh

perf-test:	perf-test-count perf-test-join

perf-test-count:
	mkdir -p perf/results
	. ./perf/measure_perf_count.sh  2> ./perf/results/count.csv && cat ./perf/results/count.csv

perf-test-join:
	mkdir -p perf/results
	. ./perf/measure_perf_join.sh  2> ./perf/results/join.csv && cat ./perf/results/join.csv
