#ifndef UTILITY_H
#define UTILITY_H

#include "constants.h"

typedef struct IfInfo {
    int index;
    char name[IF_NAME_LEN];
    unsigned char mac[MAC_LEN];
} IfInfo_t;

char* packAppData(char* data, const char* IP, const unsigned short int port, const char* msg, const unsigned char flag);
void unpackAppData(char* data, char* IP, unsigned short int* port, char* msg, unsigned char* flag);
char* getVmIPByIndex(char* ip, const int index);
int getLocalVmIndex();
void getLocalVmIP(char* localIP);
int getLocalIfInfo(IfInfo_t* localIf);
int getVmIndexByIP(const char *IP);
void prtErr(const char* msg);
void errExit(const char* msg);
void prtItemInt(const char* key, const int value);
void prtItemStr(const char* key, const char* value);
unsigned char hexStr2UChar(char const* str);
void prtMsg(const char* msg);
void prtln();

#ifdef DEBUG
void prtMac(const char *title, const unsigned char* mac);
#endif
#endif
