#ifndef ODR_H
#define ODR_H

void onRawSockAvailable(const int iRawSock);
void onDomSockAvailable(const int iDomSock, const int iRawSock, const int iIsStale);
int sendRawFrame(const int iSockfd, unsigned char* destAddr, unsigned char* srcAddr, void* data);
void setMacAddr(unsigned char* target, unsigned char* src);

#endif
