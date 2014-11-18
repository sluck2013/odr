#ifndef API_H
#define API_H

void msg_send(int sockfd, char* destIP, int destPort, char* msg, int flag);
void msg_recv(int sockfd, char* msgBuf, char* srcIP, int* srcPort);

#endif
