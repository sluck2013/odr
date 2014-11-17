struct OdrProtMsg {
	int type;
	char * srcIp;
	int srcPort;
	char * destIp;
	int destPort;
	int hopCnt;
	char Msg[2];
};

struct RREQ {
	char * srcIp;
	int broadId;
	char * destIp;
	int hopCnt;
};

struct RREP {
	char * srcIp;
	char * destIp;
	int hopCnt;
	int lifetime;
};




void RREQ();
