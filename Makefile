
BUILDIR = build

SRC=./src/
ODIR=build

__OBJ = $(wildcard -r src/*.c) $(wildcard -r src/**/*.c) $(wildcard -r src/**/**/*.c)
_OBJ = $(patsubst %.c, %.o, $(__OBJ))
OBJ = $(patsubst src/%, $(ODIR)/%, $(_OBJ))


CC = gcc
CFLAGS = -Wall -Wextra #-Werror



squel: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(ODIR)/$@

$(ODIR)/%.o: $(SRC)%.c
	$(CC) -g -c $< -o $@  $(CFLAGS)

dirs:
	mkdir -p build/parser
	mkdir -p build/planner/operators
	mkdir -p build/binder
	mkdir -p build/io
	mkdir -p build/executor
	mkdir -p build/buffer
	mkdir -p build/operators

clean:
	rm -f ./build/squel $(OBJ)

# Run tests
tests:
	./test/bats-core/bin/bats ./test