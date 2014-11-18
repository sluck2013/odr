#include "api.h"
#include "utility.h"
#include <stdio.h>

void msg_send(int sockfd, char* destIP, int destPort, char* msg, int flag) {
	char data[100];

	packAppData(data, "555", 6, "msg", 0 );
	printf("result is %s\n", data);
	char *a;
	int *b;
	char *c;
	unpackAppData(data, a, b,c);
	printf("unpack result %s%d%s \n", a,*b,c);
}
	


void msg_recv(int sockfd, char* msgBuf, char* srcIP, int* srcPort) {
}

