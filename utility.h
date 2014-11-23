#ifndef UTILITY_H
#define UTILITY_H

#include "constants.h"

char* packAppData(char* data, const char* IP, const int port, const char* msg, const int flag);
void unpackAppData(char* data, char* IP, int* port, char* msg, int* flag);
char* getVmIPByIndex(char* ip, const int index);
int getLocalVmIndex();
void getLocalVmIP(char* localIP);
void getLocalVmMac(unsigned char* localMac);
int getVmIndexByIP(const char *IP);
void prtErr(const char* msg);
void errExit(const char* msg);
void prtItemInt(const char* key, const int value);
void prtItemStr(const char* key, const char* value);
unsigned char hexStr2UChar(char const* str);

#ifdef DEBUG
void prtMac(const char *title, const unsigned char* mac);
#endif
#endif
