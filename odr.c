#include "odr.h"
#include "unp.h"
#include "utility.h"
#include "constants.h"
#include "stdio.h"
#include "RouteTable.h"
#include "PathTable.h"

PTab_t *ptPathTable;

int main(int argc, char** argv) {
    if (argc != 2) {
        char msg[MAXLINE];
        sprintf(msg, "Usage: %s <staleness>", argv[0]);
        errExit(msg);
    }

    // init path table
    ptPathTable = createPathTable();
    // add well-known port & path to path table
    addToPathTable(ptPathTable, ODR_WK_PORT, ODR_WK_PATH, 0);
    addToPathTable(ptPathTable, SERV_WK_PORT, SERV_WK_PATH, 0);

    struct sockaddr_un suOdrAddr;
    int iDomSock = socket(AF_LOCAL, SOCK_DGRAM, 0);
    bzero(&suOdrAddr, sizeof(suOdrAddr));
    suOdrAddr.sun_family = AF_LOCAL;
    strcpy(suOdrAddr.sun_path, ODR_WK_PATH);
    unlink(ODR_WK_PATH);
    Bind(iDomSock, (SA*)&suOdrAddr, sizeof(suOdrAddr));

    char pcReadBuf[MAXLINE + 1];
    char destIP[IP_LEN];
    int destPort, flag;
    char msg[2];
    struct sockaddr_un suSrcAddr;
    socklen_t len = sizeof(suSrcAddr);
    while(1) {
        int n = recvfrom(iDomSock, pcReadBuf, MAXLINE, 0, (SA*)&suSrcAddr, &len);
#ifdef DEBUG
        printf("Received %d bytes from application.\n", n);
#endif
        if (findPTabEntByPath(ptPathTable, suSrcAddr.sun_path) == NULL) {
            addToPathTable(ptPathTable, getNewPTabPort(ptPathTable), suSrcAddr.sun_path, PTAB_ENT_LIFETIME);
        }
        unpackAppData(pcReadBuf, destIP, &destPort, msg, &flag);
#ifdef DEBUG
        prtItemStr("destIP", destIP);
        prtItemInt("destPort", destPort);
        prtItemStr("msg", msg);
        prtItemInt("flag", flag);
#endif
    }
    unlink(ODR_WK_PATH);
    return 0;
}





