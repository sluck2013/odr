#ifndef ODRPROTMSG_H
#define ODRPROTMSG_H

#include "constants.h"

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
typedef struct RREQ {
    int type;
	char srcIP[IP_LEN];
	unsigned long int broadID;
	char destIP[IP_LEN];
	unsigned int hopCnt;
} RREQ_t;

//type 1
typedef struct RREP {
    int type;
	char srcIP[IP_LEN];
	char destIP[IP_LEN];
	unsigned int hopCnt;
	//unsigned int lifetime;
} RREP_t;


void makeAppMsg(AppMsg_t *AppMsg, char* srcIP, int srcPort, char* destIP, int destPort, char* msg);
void makeRREQ(RREQ_t *RREQ, const char* destIP, const unsigned long int broadID);
void makeRREP(RREP_t *RREP, char* srcIP, char* destIP);


#endif
