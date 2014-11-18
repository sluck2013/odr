#ifndef UTILITY_H
#define UTILITY_H


void packAppData(int sockfd, char* destIP, int destPort, char* msg, int flag);
char* getVmIP(char* ip, const int index);
int getVmIndex();
int getVmIndexByIP(const char *IP);
void prtErr(const char* msg);
void errExit(const char* msg);
void prtItemInt(const char* key, const int value);
void prtItemString(const char* key, const char* value);

#endif
