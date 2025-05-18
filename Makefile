CC = gcc
CFLAGS = -std=gnu17 -Wall -Wextra


EXEC = server
SRC = server.c helpers.c wrappers.c
OBJ = $(SRC:.c=.o)


all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

wrappers.o: wrappers.c wrappers.h
	$(CC) $(CFLAGS) -c wrappers.c -o wrappers.o

helpers.o: helpers.c helpers.h
	$(CC) $(CFLAGS) -c helpers.c -o helpers.o

transport.o: server.c 
	$(CC) $(CFLAGS) -c server.c -o transport.o


clean:
	rm -f $(OBJ)

distclean: clean
	rm -f $(OBJ) $(EXEC)



test: 
	./server 2137 .
