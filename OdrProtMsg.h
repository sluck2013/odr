#ifndef ODRPROTMSG_H
#define ODRPROTMSG_H

#include "constants.h"
#include "stdlib.h"
#include "string.h"

//type 2
typedef struct AppMsg {
	unsigned char type;
	int srcPort;
	int destPort;
	unsigned short int hopCnt;
	char srcIP[IP_LEN];
	char destIP[IP_LEN];
	char msg[MSG_LEN];
} AppMsg_t;

//type 0
//if RREQ changes, RREQ_SIZE should be changed accordingly
typedef struct RREQ {
    unsigned char type;
    unsigned char isResponsed;
	unsigned long int broadID;
	unsigned short int hopCnt;
	char srcIP[IP_LEN];
	char destIP[IP_LEN];
} RREQ_t;

//type 1
typedef struct RREP {
    unsigned char type;
	unsigned short int hopCnt;
	char srcIP[IP_LEN];
	char destIP[IP_LEN];
	//unsigned int lifetime;
} RREP_t;


void makeRREQ(RREQ_t *RREQ, const char* destIP, const unsigned long int broadID);
void marshalRREQ(void* dest, const RREQ_t* RREQ);
void unmarshalRREQ(RREQ_t* RREQ, const void* src);
unsigned short int incRREQHopCnt(RREQ_t* RREQ);
void prtRREQ(const RREQ_t *RREQ);
void setBroadID(RREQ_t* RREQ, const unsigned long int broadID);
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
