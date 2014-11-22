#ifndef ODR_H
#define ODR_H
#include "OdrProtMsg.h"

void onRawSockAvailable();
void onDomSockAvailable(const int iIsStale);
void onRecvRREQ(const RREQ_t *RREQ);
int sendRawFrame(const int iSockfd, unsigned char* destAddr, unsigned char* srcAddr, void* data);
void setMacAddr(unsigned char* target, unsigned char* src);

#endif
