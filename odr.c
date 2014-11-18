#include "odr.h"
#include "unp.h"
#include "utility.h"
#include "constants.h"
#include "stdio.h"

PTab_t ptPathTable;

int main(int argc, char** argv) {
    if (argc != 2) {
        char msg[MAXLINE];
        sprintf(msg, "Usage: %s <staleness>", argv[0]);
        errExit(msg);
    }

    // init path table
    ptPathTable = *createPathTable();
    // add well-known port & path to path table
    addToPathTable(&ptPathTable, ODR_WK_PORT, ODR_WK_PATH, 0);
    addToPathTable(&ptPathTable, SERV_WK_PORT, SERV_WK_PATH, 0);

    struct sockaddr_un suOdrAddr;
    int iDomSock = socket(AF_LOCAL, SOCK_DGRAM, 0);
    bzero(&suOdrAddr, sizeof(suOdrAddr));
    suOdrAddr.sun_family = AF_LOCAL;
    strcpy(suOdrAddr.sun_path, ODR_WK_PATH);
    unlink(ODR_WK_PATH);
    Bind(iDomSock, (SA*)&suOdrAddr, sizeof(suOdrAddr));

    char pcReadBuf[MAXLINE + 1];
    struct sockaddr_un suSrcAddr;
    socklen_t len = sizeof(suSrcAddr);
    while(1) {
        int n = recvfrom(iDomSock, pcReadBuf, MAXLINE, 0, (SA*)&suSrcAddr, &len);
#ifdef DEBUG
        printf("Received %d bytes from application.\n", n);
#endif
    }
    unlink(ODR_WK_PATH);
    return 0;
}

PTab_t *createPathTable() {
    PTab_t* p = malloc(sizeof(*p));
    p->head = NULL;
    p->tail = NULL;
    return p;
}

PTabElem_t *addToPathTable(PTab_t* pathTable, const unsigned int port, const char* path, const unsigned int lifetime) {
    PTabElem_t *p = malloc(sizeof(*p));
    p->createTime = time(NULL);
    p->lifetime = lifetime;
    p->port = port;
    p->next = NULL;
    p->prev = pathTable->tail;
    strcpy(p->sunPath, path);
    if (pathTable->tail == NULL) {
        pathTable->head = p;
        pathTable->tail = p;
    } else {
        pathTable->tail->next = p;
    }
    return p;
}
