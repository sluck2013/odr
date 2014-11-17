CC = gcc
LIBS = ../unpv13e/libunp.a
FLAGS = -g -O2 -Wno-unused-result

all: client
client: client.o
	$(CC) -o client client.o $(LIBS)
client.o: client.c client.h
	$(CC) $(FLAGS) -c client.c

clean:
	rm client
	rm *.o	
