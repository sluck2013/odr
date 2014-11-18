#include "odr.h"
#include "unp.h"
#include "utility.h"
#include "constants.h"

int main(int argc, char** agrv) {
    if (argc != 1) {
        errExit("Usage: ./ODR_cse533-18 <staleness>");
    }

    struct sockaddr_un suOdrAddr;
    int iDomSock = socket(AF_LOCAL, SOCK_DGRAM, 0);
    bzero(&suOdrAddr, sizeof(suOdrAddr));
    suOdrAddr.sun_family = AF_LOCAL;
    strcpy(suOdrAddr.sun_path, ODR_WK_PORT);
    unlink(ODR_WK_PORT);
    Bind(iDomSock, (SA*)&suOdrAddr, sizeof(suOdrAddr));

    char pcReadBuf[MAXLINE + 1];
    struct sockaddr_in siSrcAddr;
    socklen_t len = sizeof(siSrcAddr);
    while(1) {
        int n = recvfrom(iDomSock, pcReadBuf, MAXLINE, 0, (SA*)&siSrcAddr, &len);
        printf("Received %d bytes from application.\n", n);
    }
}
