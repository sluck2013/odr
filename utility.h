#ifndef UTILITY_H
#define UTILITY_H


void packAppData(const char* destIP, const int destPort, const char* msg, const int flag);
void unpackAppData(char* destIP, int* destPort, char* msg);
char* getVmIP(char* ip, const int index);
int getVmIndex();
int getVmIndexByIP(const char *IP);
void prtErr(const char* msg);
void errExit(const char* msg);
void prtItemInt(const char* key, const int value);
void prtItemString(const char* key, const char* value);

#endif
