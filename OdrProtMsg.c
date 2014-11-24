#include "OdrProtMsg.h"
#include "utility.h"
#include "string.h"
#include "stdio.h"

/*
inline void* memcpyItem(void* dest, void*src, size_t num) {
    memcpy(dest, src, num);
    return dest + num
}*/

void makeRREQ(RREQ_t *RREQ, const char* destIP, const unsigned long int broadID) {
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
}

void marshalRREQ(void* dest, const RREQ_t* RREQ) {
    memcpy(dest, (void*)&RREQ->type, sizeof(RREQ->type));
    memcpy(dest + 1, (void*)&RREQ->isResponsed, sizeof(RREQ->isResponsed));
    memcpy(dest + 2, (void*)&RREQ->broadID, sizeof(RREQ->broadID));
    memcpy(dest + 10, (void*)&RREQ->hopCnt, sizeof(RREQ->hopCnt));
    memcpy(dest + 12, (void*)RREQ->srcIP, IP_LEN);
    memcpy(dest + 12 + IP_LEN, (void*)RREQ->destIP, IP_LEN);
}

void unmarshalRREQ(RREQ_t* RREQ, const void* src) {
    memcpy((void*)&RREQ->type, src, sizeof(RREQ->type));
    memcpy((void*)&RREQ->isResponsed, src + 1, sizeof(RREQ->isResponsed));
    memcpy((void*)&RREQ->broadID, src + 2, sizeof(RREQ->broadID));
    memcpy((void*)&RREQ->hopCnt, src + 10, sizeof(RREQ->hopCnt));
    memcpy((void*)RREQ->srcIP, src + 12, IP_LEN);
    memcpy((void*)RREQ->destIP, src + 12 + IP_LEN, IP_LEN);
}

unsigned short int incRREQHopCnt(RREQ_t* RREQ) {
    return ++RREQ->hopCnt;
}

void setBroadID(RREQ_t* RREQ, const unsigned long int broadID) {
    RREQ->broadID = broadID;
}

void setRespBit(RREQ_t* RREQ) {
    RREQ->isResponsed = 1;
}

int isResponsed(const RREQ_t* RREQ) {
    return RREQ->isResponsed;
}

void prtRREQ(const RREQ_t *RREQ) {
    printf("broadID: %lu, hopCnt: %u, src: %s, dest: %s, replied: %d\n",
            RREQ->broadID, RREQ->hopCnt, RREQ->srcIP, RREQ->destIP, RREQ->isResponsed);
}


void makeRREP(RREP_t *RREP, const RREQ_t *RREQ, const unsigned short int hopCnt) {
    strcpy(RREP->srcIP, RREQ->srcIP);
    strcpy(RREP->destIP, RREQ->destIP);
    RREP->hopCnt = hopCnt;
    RREP->type = 1;
}

void marshalRREP(void* dest, const RREP_t* RREP) {
    memcpy(dest, (void*)&RREP->type, sizeof(RREP->type));
    memcpy(dest + 1, (void*)&RREP->hopCnt, sizeof(RREP->hopCnt));
    memcpy(dest + 3, (void*)&RREP->srcIP, sizeof(RREP->srcIP));
    memcpy(dest + 3 + IP_LEN, (void*)RREP->destIP, sizeof(RREP->destIP));
}

void unmarshalRREP(RREP_t* RREP, const void* src) {
    memcpy((void*)&RREP->type, src, sizeof(RREP->type));
    memcpy((void*)&RREP->hopCnt, src + 1, sizeof(RREP->hopCnt));
    memcpy((void*)RREP->srcIP, src + 3, IP_LEN);
    memcpy((void*)RREP->destIP, src + 3 + IP_LEN, IP_LEN);
}

void prtRREP(const RREP_t *RREP) {
    printf("hopCnt: %u, src: %s, dest: %s\n",
            RREP->hopCnt, RREP->srcIP, RREP->destIP);
}

unsigned short int incRREPHopCnt(RREP_t* RREP) {
    return ++RREP->hopCnt;
}

void makeAppMsg(AppMsg_t* appMsg, const unsigned short int srcPort, const unsigned short int destPort, const char* srcIP, const char* destIP, char* msg) {
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
    memcpy(dest + 1, (void*)&appMsg->srcPort, sizeof(appMsg->srcPort));
    memcpy(dest + 3, (void*)&appMsg->destPort, sizeof(appMsg->destPort));
    memcpy(dest + 5, (void*)&appMsg->hopCnt, sizeof(appMsg->hopCnt));
    memcpy(dest + 13, (void*)appMsg->srcIP, IP_LEN);
    memcpy(dest + 13 + IP_LEN, (void*)appMsg->destIP, IP_LEN);
    memcpy(dest + 13 + 2 * IP_LEN, (void*)appMsg->msg, MSG_LEN);
}

void unmarshalAppMsg(AppMsg_t *appMsg, const void* src) {
    memcpy((void*)&appMsg->type, src, sizeof(appMsg->type));
    memcpy((void*)&appMsg->srcPort, src + 1, sizeof(appMsg->srcPort));
    memcpy((void*)&appMsg->destPort, src + 3, sizeof(appMsg->destPort));
    memcpy((void*)&appMsg->hopCnt, src + 5, sizeof(appMsg->hopCnt));
    memcpy((void*)appMsg->srcIP, src + 13, IP_LEN);
    memcpy((void*)appMsg->destIP, src + 13 + IP_LEN, IP_LEN);
    memcpy((void*)appMsg->msg, src + 13 + 2 * IP_LEN, MSG_LEN);
}

void prtAppMsg(const AppMsg_t* appMsg) {
    printf("src: %s:%u, dest: %s:%u, hopCnt: %lu\n",
            appMsg->srcIP, appMsg->srcPort,
            appMsg->destIP, appMsg->destPort, appMsg->hopCnt);
}

unsigned short int incAppMsgHopCnt(AppMsg_t* appMsg) {
    return ++appMsg->hopCnt;
}
