#ifndef UTILITY_H
#define UTILITY_H


char* packAppData(char* data, const char* IP, const int port, const char* msg, const int flag);
void unpackAppData(char* data, char* IP, int* port, char* msg, int* flag);
char* getVmIPByIndex(char* ip, const int index);
int getLocalVmIndex();
int getVmIndexByIP(const char *IP);
void prtErr(const char* msg);
void errExit(const char* msg);
void prtItemInt(const char* key, const int value);
void prtItemStr(const char* key, const char* value);

#endif
