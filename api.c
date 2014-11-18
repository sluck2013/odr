#include "api.h"
#include "utility.h"
#include <stdio.h>
#include "unp.h"
#include "constants.h"

void msg_send(int sockfd, char* destIP, int destPort, char* msg, int flag) {
    char data[MAXLINE];
    packAppData(data, destIP, destPort, msg, flag);
    struct sockaddr_un suServAddr;
    bzero(&suServAddr, sizeof(suServAddr));
    suServAddr.sun_family = AF_LOCAL;
    strcpy(suServAddr.sun_path, ODR_WK_PATH);
    Sendto(sockfd, data, sizeof(data), 0, (SA*) &suServAddr, sizeof(suServAddr));
}
	


void msg_recv(int sockfd, char* msgBuf, char* srcIP, int* srcPort) {
	// char* packAppData(char* data,const char* destIP, const int destPort, const char* msg, const int flag) 
	// void unpackAppData(char* data, char* srcIP, int* srcPort, char* msg) 
	char data[MAXLINE];
	unpackAppData(data, srcIP, srcPort, msgBuf);
	struct sockaddr_un suCliAddr;
    	bzero(&suCliAddr, sizeof(suCliAddr));
    	suCliAddr.sun_family = AF_LOCAL;
    	strcpy(suCliAddr.sun_path, ODR_WK_PATH);
	Recvfrom(sockfd, data, sizeof(data), 0, (SA*) &suCliAddr, sizeof(suCliAddr) );
}

