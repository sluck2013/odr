#ifndef ODR_H
#define ODR_H

int sendRawFrame(const int iSockfd, unsigned char* destAddr, unsigned char* srcAddr, const unsigned char* data);
void setMacAddr(unsigned char* target, unsigned char* src);

#endif
