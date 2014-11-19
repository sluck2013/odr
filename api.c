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
	char data_from_sock[MAXLINE];
	struct sockaddr_un suServAddr;
	int nready,n;
	fd_set rset;

	FD_ZERO(&rset);

	while(1) {
	FD_SET(sockfd, &rset);
	nready=select(sockfd+1, &rset, NULL, NULL, NULL);
	if(nready<0) {
		if(errno==EINTR)
			continue;   // back to while loop
		else
			prtErr(ERR_SELECT);
	}
	
	if(FD_ISSET(sockfd, &rset))  {  // socket is readable  
		int len_suServAddr = sizeof(suServAddr);
		n=recvfrom(sockfd, data_from_sock, sizeof(data_from_sock), 0, (SA*)&suServAddr, &len_suServAddr );
		if(n<0) {
			prtErr(ERR_RECVFROM);
		}
	
		printf("start receiving message\n");
		unpackAppData(data_from_sock, srcIP, srcPort, msgBuf);
		printf("message received is %s \n", msgBuf);
	 	printf("canonical IP address is %s \n", srcIP);
	 	printf("source port number is %d \n", *srcPort);
	}	

	}
}

