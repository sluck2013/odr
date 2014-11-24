#ifndef ODR_H
#define ODR_H
#include "OdrProtMsg.h"
#include <linux/if_packet.h>

void onRawSockAvailable();
void onDomSockAvailable(const int iIsStale);
void onRecvRREQ(RREQ_t* RREQ, const struct sockaddr_ll *srcAddr);
void onRecvRREP(RREP_t* RREP, const struct sockaddr_ll *srcAddr);
void onRecvAppMsg(AppMsg_t* appMsg, const struct sockaddr_ll *srcAddr);
void floodRREQ(const int iSockfd, const int incomeIfIdx, RREQ_t *pRREQ, const int isSrc);
int sendRawFrame(const int iSockfd, const unsigned char* destAddr, const unsigned char* srcAddr, const int ifIndex, const void* data);
int getArrIdxByIfIdx(const int ifIndex);
unsigned long int bid();
void sendAppMsg(const AppMsg_t *appMsg);

#endif
