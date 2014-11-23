#ifndef ODRPROTMSG_H
#define ODRPROTMSG_H

#include "constants.h"
#include "stdlib.h"
#include "string.h"

//type 2
typedef struct AppMsg {
	int type;
	char srcIP[IP_LEN];
	int srcPort;
	char destIP[IP_LEN];
	int destPort;
	unsigned int hopCnt;
	char msg[2];
} AppMsg_t;

//type 0
//if RREQ changes, RREQ_SIZE should be changed accordingly
typedef struct RREQ {
    unsigned short int type;
	unsigned long int broadID;
	unsigned short int hopCnt;
	char srcIP[IP_LEN];
	char destIP[IP_LEN];
} RREQ_t;

//type 1
typedef struct RREP {
    unsigned short int type;
	unsigned short int hopCnt;
	char srcIP[IP_LEN];
	char destIP[IP_LEN];
	//unsigned int lifetime;
} RREP_t;


void makeRREQ(RREQ_t *RREQ, const char* destIP, const unsigned long int broadID);
void marshalRREQ(void* dest, const RREQ_t* RREQ);
void unmarshalRREQ(RREQ_t* RREQ, const void* src);
unsigned short int incHopCnt(RREQ_t* RREQ);
void prtRREQ(const RREQ_t *RREQ);
void setBroadID(RREQ_t* RREQ, const unsigned long int broadID);

void makeRREP(RREP_t *RREP, const RREQ_t *RREQ, const unsigned short int hopCnt);
void marshalRREP(void* dest, const RREP_t* RREP);
void unmarshalRREP(RREP_t* RREP, const void* src);

#endif
