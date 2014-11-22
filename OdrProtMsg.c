#include "OdrProtMsg.h"
#include "utility.h"
#include "string.h"

void makeRREQ(RREQ_t *RREQ, const char* destIP, const unsigned long int broadID) {
    char localIP[IP_LEN];
    getLocalVmIP(localIP);
    RREQ->type = 8;
    memset(RREQ->srcIP, 0, sizeof(RREQ->srcIP));
    strcpy(RREQ->srcIP, localIP);
    memset(RREQ->destIP, 0, sizeof(RREQ->destIP));
    strcpy(RREQ->destIP, destIP);
    RREQ->broadID = broadID;
    RREQ->hopCnt = 0;
}

void parseRREQ(RREQ_t* RREQ, const void* src) {
    memcpy((void*)RREQ, src, sizeof(RREQ));
    printf("ddd%d\n", sizeof(RREQ->srcIP));
    RREQ->srcIP[sizeof(RREQ->srcIP)] = '\0';
/*    memcpy((void*)&RREQ->type, src, sizeof(RREQ->type));
    memcpy((void*)&RREQ->broadID, src, sizeof(RREQ->broadID));
    memcpy((void*)&RREQ->hopCnt, src, sizeof(RREQ->hopCnt));
    memcpy((void*)&RREQ->srcIP, src, sizeof(RREQ->srcIP));
    */
}
