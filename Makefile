CC = gcc
UNP_PATH = ../unpv13e
LIBS = $(UNP_PATH)/libunp.a 

CFLAGS = -g -O2 -std=gnu99 -Wno-unused-result
IFLAGS = -I$(UNP_PATH)/lib
FLAGS = $(IFLAGS) $(CFLAGS)

all: client_cse533-18 server_cse533-18 ODR_cse533-18

client_cse533-18: client.o utility.o get_hw_addrs.o api.o
	$(CC) $(CFLAGS) -o client_cse533-18 client.o utility.o get_hw_addrs.o api.o $(LIBS)
server_cse533-18: server.o utility.o get_hw_addrs.o api.o
	$(CC) $(CFLAGS) -o server_cse533-18 server.o utility.o get_hw_addrs.o api.o $(LIBS)
ODR_cse533-18: odr.o utility.o get_hw_addrs.o RouteTable.o PathTable.o
	$(CC) $(CFLAGS) -o ODR_cse533-18 odr.o utility.o get_hw_addrs.o RouteTable.o PathTable.o $(LIBS)

client.o: client.c client.h constants.h
	$(CC) $(FLAGS) -c client.c
server.o: server.c server.h constants.h
	$(CC) $(FLAGS) -c server.c
odr.o : odr.c odr.h constants.h
	$(CC) $(FLAGS) -c odr.c
utility.o: utility.h utility.c constants.h
	$(CC) $(FLAGS) -c utility.c
get_hw_addrs.o: lib/get_hw_addrs.c lib/hw_addrs.h
	$(CC) $(FLAGS) -c lib/get_hw_addrs.c
api.o: api.h api.c constants.h
	$(CC) $(FLAGS) -c api.c
RouteTable.o: RouteTable.c RouteTable.h constants.h
	$(CC) $(FLAGS) -c RouteTable.c
PathTable.o: PathTable.c PathTable.h constants.h
	$(CC) $(FLAGS) -c PathTable.c

clean:
	echo "Removing executable files..."
	rm -f client_cse533-18 server_cse533-18 ODR_cse533-18
	echo "Removing object files..."
	rm -f *.o
