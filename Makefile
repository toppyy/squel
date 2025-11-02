
BUILDIR = build

SRC=./src/
ODIR=build

# We only want to build the odbc-api for libsquel
# which is why this trickery exists

C_SOURCE = $(wildcard -r src/*.c) $(wildcard -r src/**/*.c) $(wildcard -r src/**/**/*.c)
C_WITHOUT_ODBC = $(filter-out $(wildcard -r src/odbc/*.c), $(C_SOURCE))
C_ODBC = $(filter $(wildcard -r src/odbc/*.c), $(C_SOURCE))

_OBJ = $(patsubst %.c, %.o, $(C_WITHOUT_ODBC))
OBJ	 		= $(patsubst src/%, $(ODIR)/%, $(_OBJ))

_ODBC_OBJ = $(patsubst %.c, %.o, $(C_ODBC))
ODBC_OBJ   = $(patsubst src/%, $(ODIR)/%, $(_ODBC_OBJ))

LIBRARY = $(BUILDIR)/libsquel.so


CC = gcc
CFLAGS = -Wall -Wextra -Werror -fPIC
#CFLAGS =  -fPIC

# Define a target to compile all source files
all: dirs squel

squel: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(ODIR)/$@

$(ODIR)/%.o: $(SRC)%.c
	$(CC) -g -c $< -o $@  $(CFLAGS)


libsquel: odbc_obj lib

odbc_obj: $(ODBC_OBJ)
	$(CC) $(CFLAGS) $(ODBC_OBJ) -o $(ODIR)/$@

$(ODIR)/%.o: $(SRC)%.c
	$(CC) -g -c $< -o $@  $(CFLAGS)

lib: $(LIBRARY) 

OBJ_WITHOUT_MAIN := $(filter-out $(ODIR)/squel.o, $(OBJ))

LIB_OBJ = $(OBJ_WITHOUT_MAIN) $(ODBC_OBJ)

$(LIBRARY): $(OBJ_WITHOUT_MAIN)
	@mkdir -p $(BUILDIR)
	$(CC) -shared $(LDFLAGS) -o $@ $^

dirs:
	mkdir -p data build/parser build/planner/operators build/binder build/io build/executor build/executor/statements build/operators build/util/hashmap build/api build/odbc

clean:
	rm -f ./build/squel $(OBJ)
	rm -f ./build/libsquel.so

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
