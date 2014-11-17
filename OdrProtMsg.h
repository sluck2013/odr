struct OdrProtMsg {
	int type;
	char * srcIp;
	int srcPort;
	char * destIp;
	int destPort;
	int hopCnt;
	char Msg[2];
};

void makeOdrProtMsg();
