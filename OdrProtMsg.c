#include "OdrProtMsg.h"
#include "utility.h"
#include "string.h"
#include "unp.h"

void makeRREQ(RREQ_t *RREQ, const char* destIP, const unsigned long int broadID) {
    char localIP[IP_LEN];
    getLocalVmIP(localIP);
    RREQ->type = htonl(0);    // network byte order
    strcpy(RREQ->srcIP, localIP);
    strcpy(RREQ->destIP, destIP);
    RREQ->broadID = htonl(broadID);
    RREQ->hopCnt = 0;
}


void makeRREP(RREP_t *RREP, char* srcIP, char* destIP) {
	//char localIP[IP_LEN];
	//getLocalVmIP(localIP);
	RREP->type=htonl(1);
	strcpy(RREP->srcIP, srcIP);
	strcpy(RREP->destIP, destIP);
	RREP->hopCnt=0;
}

void makeAppMsg(AppMsg_t *AppMsg, char* srcIP, int srcPort, char* destIP, int destPort, char* msg) {
	AppMsg->type=htonl(2);
	strcpy(AppMsg->srcIP, srcIP);
	AppMsg->srcPort=htonl(srcPort);
	strcpy(AppMsg->destIP, destIP);
	AppMsg->destPort=htonl(destPort);
	AppMsg->hopCnt=0;
	strcpy(AppMsg->msg, msg);
}