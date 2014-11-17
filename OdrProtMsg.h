#ifndef ODRPROTMSG_H
#define ODRPROTMSG_H

struct AppMsg {
	int type;
	char srcIP[IP_LEN];
	int srcPort;
	char destIP[IP_LEN];
	int destPort;
	unsigned int hopCnt;
	char msg[2];
};

struct RREQ {
    int type;
	char srcIP[IP_LEN];
	unsigned int broadID;
	char destIP[IP_LEN];
	unsigned int hopCnt;
};

struct RREP {
    int type;
	char srcIP[IP_LEN];
	char destIP[IP_LEN];
	unsigned int hopCnt;
	unsigned int lifetime;
};

void makeAppMsg();
void makeRREQ();
void makeRREP();

#endif
