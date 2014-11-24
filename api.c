#include "api.h"
#include "utility.h"
#include <stdio.h>
#include "unp.h"
#include "constants.h"

/*
 * msg_send() required by document
 */
void msg_send(int sockfd, char* destIP, int destPort, char* msg, int flag) {
    char data[APP_DATA_LEN];
    packAppData(data, destIP, destPort, msg, flag);
    struct sockaddr_un suRemoteAddr;
    bzero(&suRemoteAddr, sizeof(suRemoteAddr));
    suRemoteAddr.sun_family = AF_LOCAL;
    strcpy(suRemoteAddr.sun_path, ODR_WK_PATH);
    Sendto(sockfd, data, sizeof(data), 0, (SA*) &suRemoteAddr, sizeof(suRemoteAddr));
}

/*
 * msg_recv() required by document
 */
void msg_recv(int sockfd, char* msgBuf, char* srcIP, int* srcPort) {
    char data[APP_DATA_LEN];
    struct sockaddr_un suSrcAddr;
    socklen_t len = sizeof(suSrcAddr);
    int n = recvfrom(sockfd, data, APP_DATA_LEN, 0, (SA*)&suSrcAddr, &len);
    unpackAppData(data, srcIP, srcPort, msgBuf, NULL);
}

