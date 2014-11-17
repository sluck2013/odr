#ifndef UTILITY_H
#define UTILITY_H

void packAppData(int sockfd, char* destIP, int destPort, char* msg, int flag);
int getVmIndex();
void prtErr(const char* msg);
void errExit(const char* msg);
void prtItemInt(const char* key, const int value);

#endif
