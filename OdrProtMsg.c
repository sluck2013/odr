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
}

void marshalRREQ(void* dest, const RREQ_t* RREQ) {
    memcpy(dest, (void*)&RREQ->type, sizeof(RREQ->type));
    memcpy(dest + 2, (void*)&RREQ->broadID, sizeof(RREQ->broadID));
    memcpy(dest + 6, (void*)&RREQ->hopCnt, sizeof(RREQ->hopCnt));
    memcpy(dest + 8, (void*)RREQ->srcIP, sizeof(RREQ->srcIP));
    memcpy(dest + 8 + IP_LEN, (void*)RREQ->destIP, sizeof(RREQ->destIP));
}

void unmarshalRREQ(RREQ_t* RREQ, const void* src) {
    memcpy((void*)&RREQ->type, src, sizeof(RREQ->type));
    memcpy((void*)&RREQ->broadID, src + 2, sizeof(RREQ->broadID));
    memcpy((void*)&RREQ->hopCnt, src + 6, sizeof(RREQ->hopCnt));
    memcpy((void*)RREQ->srcIP, src + 8, sizeof(RREQ->srcIP));
    memcpy((void*)RREQ->destIP, src + 8 + IP_LEN, sizeof(RREQ->srcIP));
}

unsigned short int incHopCnt(RREQ_t* RREQ) {
    return ++RREQ->hopCnt;
}


void makeRREP(RREP_t *RREP, const RREQ_t *RREQ, const unsigned short int hopCnt) {
    strcpy(RREP->srcIP, RREQ->srcIP);
    strcpy(RREP->destIP, RREQ->destIP);
    RREP->hopCnt = hopCnt;
}

void marshalRREP(void* dest, const RREP_t* RREP) {
    memcpy(dest, (void*)&RREP->type, sizeof(RREP->type));
    memcpy(dest + 2, (void*)&RREP->hopCnt, sizeof(RREP->hopCnt));
    memcpy(dest + 4, (void*)&RREP->srcIP, sizeof(RREP->srcIP));
    memcpy(dest + 4 + IP_LEN, (void*)RREP->destIP, sizeof(RREP->destIP));
}

void unmarshalRREP(RREP_t* RREP, const void* src) {
}
