CC = gcc
UNP_PATH = ../unpv13e
LIBS = $(UNP_PATH)/libunp.a 

CFLAGS = -g -O2 -std=gnu99 -Wno-unused-result
IFLAGS = -I$(UNP_PATH)/lib
FLAGS = $(IFLAGS) $(CFLAGS)

all: client_cse533-18

client_cse533-18: client.o utility.o get_hw_addrs.o
	$(CC) $(CFLAGS) -o client_cse533-18 client.o utility.o get_hw_addrs.o $(LIBS)
client.o: client.c client.h constants.h
	$(CC) $(FLAGS) -c client.c
utility.o: utility.h utility.c constants.h
	$(CC) $(FLAGS) -c utility.c 
get_hw_addrs.o: lib/get_hw_addrs.c lib/hw_addrs.h
	$(CC) $(FLAGS) -c lib/get_hw_addrs.c

clean:
	echo "Removing executable files..."
	rm -f client_cse533-18
	echo "Removing object files..."
	rm -f *.o
