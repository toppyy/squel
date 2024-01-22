SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

CC = gcc
#CFLAGS = -Wall -Wextra -Werror
##$(CC) $(CFLAGS) $(OBJ) -o $@
CFLAGS = 
squel: $(OBJ)
	$(CC) $(OBJ) -o $@


clean:
	rm -f squel $(OBJ)
