#include "odr.h"
#include "unp.h"
#include "utility.h"
#include "constants.h"
#include "stdio.h"
#include "RouteTable.h"
#include "PathTable.h"
#include "OdrProtMsg.h"
#include "ctype.h"
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>

PTab_t *pPathTab;
RTab_t *pRouteTab;
unsigned long int ulBroadID = 0;

int main(int argc, char** argv) {
    if (argc != 2) {
        char msg[MAXLINE];
        sprintf(msg, "Usage: %s <staleness>", argv[0]);
        errExit(msg);
    }

    int isStale = 0; //TODO

    // init path table
    pPathTab = createPathTable();
    // add well-known port & path to path table
    addToPathTable(pPathTab, ODR_WK_PORT, ODR_WK_PATH, 0);
    addToPathTable(pPathTab, SERV_WK_PORT, SERV_WK_PATH, 0);
    pRouteTab = createRouteTable();

    struct sockaddr_un suOdrAddr;
    int iDomSock = socket(AF_LOCAL, SOCK_DGRAM, 0);
    bzero(&suOdrAddr, sizeof(suOdrAddr));
    suOdrAddr.sun_family = AF_LOCAL;
    strcpy(suOdrAddr.sun_path, ODR_WK_PATH);
    unlink(ODR_WK_PATH);
    Bind(iDomSock, (SA*)&suOdrAddr, sizeof(suOdrAddr));

    char pcReadBuf[MAXLINE + 1];
    char destIP[IP_LEN];
    int destPort, flag;
    char msg[2];
    struct sockaddr_un suSrcAddr;
    socklen_t len = sizeof(suSrcAddr);
    while(1) {
        int n = recvfrom(iDomSock, pcReadBuf, MAXLINE, 0, (SA*)&suSrcAddr, &len);
#ifdef DEBUG
        printf("Received %d bytes from application.\n", n);
#endif
        if (findPTabEntByPath(pPathTab, suSrcAddr.sun_path) == NULL) {
            addToPathTable(pPathTab, getNewPTabPort(pPathTab), suSrcAddr.sun_path, PTAB_ENT_LIFETIME);
        }
        unpackAppData(pcReadBuf, destIP, &destPort, msg, &flag);
#ifdef DEBUG
        prtItemStr("destIP", destIP);
        prtItemInt("destPort", destPort);
        prtItemStr("msg", msg);
        prtItemInt("flag", flag);
#endif

        RTabEnt_t *prEnt = getRTabEntByDest(pRouteTab, destIP);
        if (prEnt == NULL || flag > 0 || isStale) {
            //send RREQ
            RREQ_t RREQ;
            makeRREQ(&RREQ, destIP, ulBroadID++);
            char localMac[MAC_LEN];
            getLocalVmMac(localMac);
#ifdef DEBUG
            prtItemStr("Str local Mac", localMac);
#endif
            //create socket
            char destMac[MAC_LEN] = ODR_BROADCAST_MAC;
            sendRawFrame(111, destMac, localMac, (unsigned char*) & RREQ);
        } else {
            //send DATA
        }
    }
    unlink(ODR_WK_PATH);
    return 0;
}

int sendRawFrame(const int iSockfd, unsigned char* destAddr, unsigned char* srcAddr, const unsigned char* data) {
    struct sockaddr_ll sockAddr;
    void* buffer = (void*)malloc(ETH_FRAME_LEN);
    unsigned char* etherhead = buffer;
    unsigned char* usrData = buffer + 14;
    struct ethhdr *eh = (struct ethhdr *)etherhead;;
    unsigned char srcMac[6], destMac[6];
    setMacAddr(srcMac, srcAddr);
    setMacAddr(destMac, destAddr);
#ifdef DEBUG
    prtMac("srcMac", srcMac);
    prtMac("destMac", destMac);
#endif

    /*prepare sockaddr_ll*/
    sockAddr.sll_family   = PF_PACKET;	
    sockAddr.sll_protocol = htons(ETH_P_IP); //not in use	
    sockAddr.sll_ifindex  = ETH_IF_INDEX;
    sockAddr.sll_hatype   = ARPHRD_ETHER;
    sockAddr.sll_pkttype  = PACKET_OTHERHOST;
    sockAddr.sll_halen    = ETH_ALEN;		
    /*MAC - begin*/
    for (int i = 0; i < 6; ++i) {
        sockAddr.sll_addr[i] = destMac[i];
    }
    /*MAC - end*/
    sockAddr.sll_addr[6]  = 0x00;/*not used*/
    sockAddr.sll_addr[7]  = 0x00;/*not used*/

    /*set the frame header*/
    memcpy((void*)buffer, (void*)destMac, ETH_ALEN);
    memcpy((void*)(buffer+ETH_ALEN), (void*)srcMac, ETH_ALEN);
    eh->h_proto = 0x00;
    /*fill the frame with some data*/
    memcpy(usrData, data, sizeof(data));

    /*send the packet*/
    int n = sendto(iSockfd, buffer, ETH_FRAME_LEN, 0, 
            (struct sockaddr*)&sockAddr, sizeof(sockAddr));
    if (n == -1) {
        prtErr(ERR_SEND_RAW_DATA);
    }
    return n;
}

void setMacAddr(unsigned char* target, unsigned char* src) {
    target[0] = hexStr2UChar(strtok(src, ":"));
    for (int i = 1; i < 5; ++i) {
        target[i] = hexStr2UChar(strtok(NULL, ":"));
    }
    target[5] = hexStr2UChar(&src[15]);
}
