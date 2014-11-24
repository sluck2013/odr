#include "OdrProtMsg.h"
#include "utility.h"
#include "string.h"
#include "stdio.h"
#include "unp.h"

/*
inline void* memcpyItem(void* dest, void*src, size_t num) {
    memcpy(dest, src, num);
    return dest + num
}*/

void makeRREQ(RREQ_t *RREQ, const char* destIP, const unsigned int broadID, const unsigned char forceDiscovery) {
    char localIP[IP_LEN];
    getLocalVmIP(localIP);
    RREQ->type = 0;
    memset(RREQ->srcIP, 0, sizeof(RREQ->srcIP));
    strcpy(RREQ->srcIP, localIP);
    memset(RREQ->destIP, 0, sizeof(RREQ->destIP));
    strcpy(RREQ->destIP, destIP);
    RREQ->broadID = broadID;
    RREQ->hopCnt = 0;
    RREQ->isResponsed = 0;
    RREQ->forceDisc = forceDiscovery;
}

void marshalRREQ(void* dest, const RREQ_t* RREQ) {
    memcpy(dest, (void*)&RREQ->type, sizeof(RREQ->type));
    memcpy(dest + 1, (void*)RREQ->srcIP, IP_LEN);
    memcpy(dest + 1 + IP_LEN, (void*)RREQ->destIP, IP_LEN);
    memcpy(dest + 1 + 2 * IP_LEN, (void*)&RREQ->isResponsed, sizeof(RREQ->isResponsed));
    memcpy(dest + 2 + 2 * IP_LEN, (void*)&RREQ->forceDisc, sizeof(RREQ->forceDisc));
    unsigned int broadID = htonl(RREQ->broadID);
    memcpy(dest + 3 + 2 * IP_LEN, (void*)&broadID, sizeof(RREQ->broadID));
    unsigned short int hopCnt = htons(RREQ->hopCnt);
    memcpy(dest + 7 + 2 * IP_LEN, (void*)&hopCnt, sizeof(RREQ->hopCnt));
}

void unmarshalRREQ(RREQ_t* RREQ, const void* src) {
    unsigned int broadID;
    unsigned short int hopCnt;
    memcpy((void*)&RREQ->type, src, sizeof(RREQ->type));
    memcpy((void*)RREQ->srcIP, src + 1, IP_LEN);
    memcpy((void*)RREQ->destIP, src + 1 + IP_LEN, IP_LEN);
    memcpy((void*)&RREQ->isResponsed, src + 1 + 2 * IP_LEN, sizeof(RREQ->isResponsed));
    memcpy((void*)&RREQ->forceDisc, src + 2 + 2 * IP_LEN, sizeof(RREQ->forceDisc));
    memcpy((void*)&broadID, src + 3, sizeof(RREQ->broadID));
    memcpy((void*)&hopCnt, src + 11, sizeof(RREQ->hopCnt));
    RREQ->broadID = ntohl(broadID);
    RREQ->hopCnt = ntohs(hopCnt);
}

unsigned short int incRREQHopCnt(RREQ_t* RREQ) {
    return ++RREQ->hopCnt;
}

void setBroadID(RREQ_t* RREQ, const unsigned int broadID) {
    RREQ->broadID = broadID;
}

void setRespBit(RREQ_t* RREQ) {
    RREQ->isResponsed = 1;
}

int isResponsed(const RREQ_t* RREQ) {
    return RREQ->isResponsed;
}

void prtRREQ(const RREQ_t *RREQ) {
    printf("broadID: %u, hopCnt: %u, src: %s, dest: %s, replied: %d, forceDiscovery: %u\n",
            RREQ->broadID, RREQ->hopCnt, RREQ->srcIP, RREQ->destIP, RREQ->isResponsed, RREQ->forceDisc);
}


void makeRREP(RREP_t *RREP, const RREQ_t *RREQ, const unsigned short int hopCnt) {
    strcpy(RREP->srcIP, RREQ->srcIP);
    strcpy(RREP->destIP, RREQ->destIP);
    RREP->hopCnt = hopCnt;
    RREP->type = 1;
    RREP->forceDisc = RREQ->forceDisc;
}

void marshalRREP(void* dest, const RREP_t* RREP) {
    memcpy(dest, (void*)&RREP->type, sizeof(RREP->type));
    memcpy(dest + 1, (void*)&RREP->srcIP, sizeof(RREP->srcIP));
    memcpy(dest + 1 + IP_LEN, (void*)RREP->destIP, sizeof(RREP->destIP));
    memcpy(dest + 1 + 2 * IP_LEN, (void*)&RREP->forceDisc, sizeof(RREP->forceDisc));
    unsigned short int hopCnt = htons(RREP->hopCnt);
    memcpy(dest + 2 + 2 * IP_LEN, (void*)&hopCnt, sizeof(RREP->hopCnt));
}

void unmarshalRREP(RREP_t* RREP, const void* src) {
    unsigned short int hopCnt;
    memcpy((void*)&RREP->type, src, sizeof(RREP->type));
    memcpy((void*)RREP->srcIP, src + 1, IP_LEN);
    memcpy((void*)RREP->destIP, src + 1 + IP_LEN, IP_LEN);
    memcpy((void*)&RREP->forceDisc, src + 1 + 2 * IP_LEN, sizeof(RREP->forceDisc));
    memcpy((void*)&hopCnt, src + 2 + 2 * IP_LEN, sizeof(RREP->hopCnt));
    RREP->hopCnt = ntohs(hopCnt);
}

void prtRREP(const RREP_t *RREP) {
    printf("hopCnt: %u, src: %s, dest: %s, forceDiscovery: %u\n",
            RREP->hopCnt, RREP->srcIP, RREP->destIP, RREP->forceDisc);
}

unsigned short int incRREPHopCnt(RREP_t* RREP) {
    return ++RREP->hopCnt;
}

void makeAppMsg(AppMsg_t* appMsg, const int srcPort, const int destPort, const char* srcIP, const char* destIP, char* msg) {
    appMsg->type = 2;
    appMsg->srcPort = srcPort;
    appMsg->destPort = destPort;
    appMsg->hopCnt = 0;
    strcpy(appMsg->srcIP, srcIP);
    strcpy(appMsg->destIP, destIP);
    strcpy(appMsg->msg, msg);
}

void marshalAppMsg(void* dest, const AppMsg_t* appMsg) {
    memcpy(dest, (void*)&appMsg->type, sizeof(appMsg->type));
    memcpy(dest + 1, (void*)appMsg->srcIP, IP_LEN);
    memcpy(dest + 1 + IP_LEN, (void*)appMsg->destIP, IP_LEN);
    int srcPort = htonl(appMsg->srcPort);
    memcpy(dest + 1 + 2 * IP_LEN, (void*)&srcPort, sizeof(appMsg->srcPort));
    int destPort = htonl(appMsg->destPort);
    memcpy(dest + 5 + 2 * IP_LEN, (void*)&destPort, sizeof(appMsg->destPort));
    unsigned short int hopCnt = htons(appMsg->hopCnt);
    memcpy(dest + 9 + 2 * IP_LEN, (void*)&hopCnt, sizeof(appMsg->hopCnt));
    memcpy(dest + 11 + 2 * IP_LEN, (void*)appMsg->msg, MSG_LEN);
}

void unmarshalAppMsg(AppMsg_t *appMsg, const void* src) {
    int srcPort, destPort;
    unsigned short int hopCnt;
    memcpy((void*)&appMsg->type, src, sizeof(appMsg->type));
    memcpy((void*)appMsg->srcIP, src + 1, IP_LEN);
    memcpy((void*)appMsg->destIP, src + 1 + IP_LEN, IP_LEN);
    memcpy((void*)&srcPort, src + 1 + 2 * IP_LEN, sizeof(appMsg->srcPort));
    appMsg->srcPort = ntohl(srcPort);
    memcpy((void*)&destPort, src + 5 + 2 * IP_LEN, sizeof(appMsg->destPort));
    appMsg->destPort = ntohl(destPort);
    memcpy((void*)&hopCnt, src + 9 + 2 * IP_LEN, sizeof(appMsg->hopCnt));
    appMsg->hopCnt = ntohs(hopCnt);
    memcpy((void*)appMsg->msg, src + 11 + 2 * IP_LEN, MSG_LEN);
}

void prtAppMsg(const AppMsg_t* appMsg) {
    printf("src: %s:%d, dest: %s:%d, hopCnt: %u\n",
            appMsg->srcIP, appMsg->srcPort,
            appMsg->destIP, appMsg->destPort, appMsg->hopCnt);
}

unsigned short int incAppMsgHopCnt(AppMsg_t* appMsg) {
    return ++appMsg->hopCnt;
}
