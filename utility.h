#ifndef UTILITY_H
#define UTILITY_H


void packAppData(char* data,const char* destIP, const int destPort, const char* msg, const int flag);
void unpackAppData(const char* data, char* srcIP, int* srcPort, char* msg);
char* getVmIP(char* ip, const int index);
int getVmIndex();
int getVmIndexByIP(const char *IP);
void prtErr(const char* msg);
void errExit(const char* msg);
void prtItemInt(const char* key, const int value);
void prtItemString(const char* key, const char* value);

#endif
