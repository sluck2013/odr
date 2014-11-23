#ifndef ODR_H
#define ODR_H
#include "OdrProtMsg.h"
#include <linux/if_packet.h>

void onRawSockAvailable();
void onDomSockAvailable(const int iIsStale);
void onRecvRREQ(RREQ_t* RREQ, const struct sockaddr_ll *srcAddr);
int sendRawFrame(const int iSockfd, const unsigned char* destAddr, const unsigned char* srcAddr, const void* data);
//void setMacAddr(unsigned char* target, unsigned char* src);

#endif
