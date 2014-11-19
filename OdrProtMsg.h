#ifndef ODRPROTMSG_H
#define ODRPROTMSG_H

//type 2
struct AppMsg {
	int type;
	char srcIP[IP_LEN];
	int srcPort;
	char destIP[IP_LEN];
	int destPort;
	unsigned int hopCnt;
	char msg[2];
};

//type 0
struct RREQ {
    int type;
	char srcIP[IP_LEN];
	unsigned int broadID;
	char destIP[IP_LEN];
	unsigned int hopCnt;
};

//type 1
struct RREP {
    int type;
	char srcIP[IP_LEN];
	char destIP[IP_LEN];
	unsigned int hopCnt;
	//unsigned int lifetime;
};

void makeAppMsg();
void makeRREQ();
void makeRREP();

#endif
