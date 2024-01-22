
BUILDIR = build

SRC=./src/
ODIR=build

__OBJ = $(wildcard -r src/*.c) $(wildcard -r src/**/*.c) $(wildcard -r src/**/**/*.c)
_OBJ = $(patsubst %.c, %.o, $(__OBJ))
OBJ = $(patsubst src/%, $(ODIR)/%, $(_OBJ))


CC = gcc
CFLAGS = -Wall -Wextra -Werror

squel: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(ODIR)/$@

$(ODIR)/%.o: $(SRC)%.c
	$(CC) -g -c $< -o $@  $(CFLAGS)



clean:
	rm -f squel $(OBJ)
