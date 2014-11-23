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

unsigned short int incHopCnt(RREQ_t* RREQ) {
    return ++RREQ->hopCnt;
}

void setBroadID(RREQ_t* RREQ, const unsigned long int broadID) {
    RREQ->broadID = broadID;
}

void setRespBit(RREQ_t* RREQ) {
    RREQ->isResponsed = 1;
}

void prtRREQ(const RREQ_t *RREQ) {
    printf("broadID: %lu, hopCnt: %u, src: %s, dest: %s, replied: %d\n",
            RREQ->broadID, RREQ->hopCnt, RREQ->srcIP, RREQ->destIP, RREQ->isResponsed);
}


void makeRREP(RREP_t *RREP, const RREQ_t *RREQ, const unsigned short int hopCnt) {
    strcpy(RREP->srcIP, RREQ->srcIP);
    strcpy(RREP->destIP, RREQ->destIP);
    RREP->hopCnt = hopCnt;
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
