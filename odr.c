#include "odr.h"
#include "unp.h"
#include "utility.h"
#include "constants.h"
#include "stdio.h"

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

PTab_t *createPathTable() {
    PTab_t* p = malloc(sizeof(*p));
    p->head = NULL;
    p->tail = NULL;
    p->portList.head = NULL;
    p->portList.tail = NULL;
    for (int i = MAX_EPORT; i >= MIN_EPORT; --i) {
        portList_pushBack(&p->portList, i);
    }
    return p;
}

PTabEnt_t *addToPathTable(PTab_t* pathTable, const unsigned int port, const char* path, const unsigned int lifetime) {
    PTabEnt_t *p = malloc(sizeof(*p));
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
        pathTable->tail = p;
    }
#ifdef DEBUG
    printf("New path table entry: <port %d, path %s>\n", port, path);
#endif
    return p;
}

PTabEnt_t *findPTabEntByPath(const PTab_t* pathTable, const char* sunPath) {
    PTabEnt_t *p = pathTable->head;
    while (p != NULL) {
        if (strcmp(sunPath, p->sunPath) == 0) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

AvailPort_t *portList_pushBack(PortList_t* plist, const unsigned int port) {
    AvailPort_t *newElem = malloc(sizeof(*newElem));
    newElem->next = NULL;
    newElem->prev = plist->tail;
    newElem->port = port;

    if (plist->tail == NULL) {
        plist->tail = newElem;
        plist->head = newElem;
    } else {
        plist->tail->next = newElem;
        plist->tail = newElem;
    }
    return newElem;
}

unsigned int portList_popFront(PortList_t* plist) {
    if (plist->head == NULL) {
        return 0;
    }
    unsigned int r = plist->head->port;
    portList_remove(plist, plist->head);
    return r;
}


void portList_remove(PortList_t* plist, AvailPort_t *target) {
    if (target != plist->head) {
        target->prev->next = target->next;
    } else {
        plist->head = target->next;
    }
    
    if (target != plist->tail) {
        target->next->prev = target->prev;
    } else {
        plist->tail = target->prev;
    }
    free(target);
}

int portList_isEmpty(PortList_t* plist) {
    return (plist->head == NULL);
}

unsigned int getNewPTabPort(PTab_t* pathTable) {
    PortList_t *pl = &pathTable->portList;
    if (portList_isEmpty(pl)) {
        prtErr(ERR_PTAB_OVERFLOW);
        return 0;
    } else {
        return portList_popFront(pl);
    }
}

RTabEnt_t *getRTabEntByDest(const RTab_t* rTable, const char* destIP) {
    RTabEnt_t *p = rTable->head;
    while (p != NULL) {
        if (strcmp(p->destIP, destIP) == 0) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

#ifdef DEBUG
void prtPortList(const PTab_t* pathTable) {
    printf("port list:\n");
    AvailPort_t *p = pathTable->portList.head;
    while (p != NULL) {
        printf("%d ", p->port);
        p = p->next;
    }
}
#endif
