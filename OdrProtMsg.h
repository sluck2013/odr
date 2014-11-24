/*
 * This file defines 3 types of ODR protocol messages.
 * If structure changes, RREQ_SIZE, RREP_SIZE and APPMSG_SIZE defined in
 * constants.h should be changed accordingly.
 */
#ifndef ODRPROTMSG_H
#define ODRPROTMSG_H

#include "constants.h"
#include "stdlib.h"
#include "string.h"

//type 2
typedef struct AppMsg {
	unsigned char type;
	char srcIP[IP_LEN];
	char destIP[IP_LEN];
	int srcPort;
	int destPort;
	unsigned short int hopCnt;
	char msg[MSG_LEN];
} AppMsg_t;

//type 0
typedef struct RREQ {
    unsigned char type;
	char srcIP[IP_LEN];
	char destIP[IP_LEN];
    unsigned char isResponsed;
    unsigned char forceDisc;
	unsigned int broadID;
	unsigned short int hopCnt;

} RREQ_t;

//type 1
typedef struct RREP {
    unsigned char type;
	char srcIP[IP_LEN];
	char destIP[IP_LEN];
    unsigned char forceDisc;
	unsigned short int hopCnt;
	//unsigned int lifetime;
} RREP_t;


void makeRREQ(RREQ_t *RREQ, const char* destIP, const unsigned int broadID, const unsigned char forceDiscovery);
void marshalRREQ(void* dest, const RREQ_t* RREQ);
void unmarshalRREQ(RREQ_t* RREQ, const void* src);
unsigned short int incRREQHopCnt(RREQ_t* RREQ);
void prtRREQ(const RREQ_t *RREQ);
void setBroadID(RREQ_t* RREQ, const unsigned int broadID);
void setRespBit(RREQ_t* RREQ);
int isResponsed(const RREQ_t* RREQ);

void makeRREP(RREP_t *RREP, const RREQ_t *RREQ, const unsigned short int hopCnt);
void marshalRREP(void* dest, const RREP_t* RREP);
void unmarshalRREP(RREP_t* RREP, const void* src);
void prtRREP(const RREP_t *RREP);
unsigned short int incRREPHopCnt(RREP_t* RREP);

void makeAppMsg(AppMsg_t* appMsg, const int srcPort, const int destPort, const char* srcIP, const char* destIP, char* msg);
void marshalAppMsg(void* dest, const AppMsg_t* appMsg);
void unmarshalAppMsg(AppMsg_t* appMsg, const void* src);
void prtAppMsg(const AppMsg_t *appMsg);
unsigned short int incAppMsgHopCnt(AppMsg_t* appMsg);

#endif
