#ifndef UTILITY_H
#define UTILITY_H

void packAppData(int sockfd, char* destIP, int destPort, char* msg, int flag);
void prtErr(char* msg);
void errExit(char* msg);

#endif
