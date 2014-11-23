#include "odr.h"
#include "unp.h"
#include "utility.h"
#include "constants.h"
#include "stdio.h"
#include "RouteTable.h"
#include "PathTable.h"
#include "OdrProtMsg.h"
#include "ctype.h"
#include "string.h"
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>

PTab_t *pPathTab;
RTab_t *pRouteTab;
unsigned long int ulBroadID = 0;
int iDomSock = 0, iRawSock = 0;

int main(int argc, char** argv) {
    if (argc != 2) {
        char msg[MAXLINE];
        sprintf(msg, "Usage: %s <staleness>", argv[0]);
        errExit(msg);
    }

    int iIsStale = 0; //TODO

    // init path table
    pPathTab = createPathTable();
    // add well-known port & path to path table
    addToPathTable(pPathTab, ODR_WK_PORT, ODR_WK_PATH, 0);
    addToPathTable(pPathTab, SERV_WK_PORT, SERV_WK_PATH, 0);
    pRouteTab = createRouteTable();

    struct sockaddr_un suOdrAddr;
    iDomSock = socket(AF_LOCAL, SOCK_DGRAM, 0);
    bzero(&suOdrAddr, sizeof(suOdrAddr));
    suOdrAddr.sun_family = AF_LOCAL;
    strcpy(suOdrAddr.sun_path, ODR_WK_PATH);
    unlink(ODR_WK_PATH);
    Bind(iDomSock, (SA*)&suOdrAddr, sizeof(suOdrAddr));

    iRawSock = socket(PF_PACKET, SOCK_RAW, htons(ETH_PROT_VALUE));
    if (iRawSock == -1) {
        errExit(ERR_CREATE_RAW_SOCK);
    }

    int iMaxFd = max(iDomSock, iRawSock);
    fd_set fsRSet, fsAllSet;
    FD_ZERO(&fsAllSet);
    FD_SET(iDomSock, &fsAllSet);
    FD_SET(iRawSock, &fsAllSet);

    while (1) {
        fsRSet = fsAllSet;
        select(iMaxFd + 1, &fsRSet, NULL, NULL, NULL); 
        if FD_ISSET(iDomSock, &fsRSet) {
            onDomSockAvailable(iIsStale);
        }
        if FD_ISSET(iRawSock, &fsRSet) {
            onRawSockAvailable();
        }
    }
    unlink(ODR_WK_PATH);
}

void onRawSockAvailable() {
    struct sockaddr_ll srcAddr;
    int len = sizeof(srcAddr);
    void* buffer = malloc(ETH_FRAME_LEN);
    int n = recvfrom(iRawSock, buffer, ETH_FRAME_LEN, 0, (SA*)&srcAddr, &len);
    if (n < 0) {
        prtErr(ERR_RAW_RECV);
    }
    unsigned short int iMsgType;
    memcpy((void*)&iMsgType, buffer + ETH_DATA_OFFSET, sizeof(iMsgType));
    switch (iMsgType) {
        case 0:
#ifdef DEBUG
            printf("Received RREQ\n");
#endif
            RREQ_t RREQ;
            unmarshalRREQ(&RREQ, buffer + ETH_DATA_OFFSET);
            free(buffer);
            onRecvRREQ(&RREQ, &srcAddr);
            break;
        case 1:
#ifdef DEBUG
            
#endif
            break;
        case 2:
#ifdef DEBUG
#endif
            break;
        default:
            break;
    }
}

void onDomSockAvailable(const int iIsStale) {
    char pcReadBuf[MAXLINE + 1];
    char destIP[IP_LEN];
    int destPort, flag;
    char msg[2];
    struct sockaddr_un suSrcAddr;
    socklen_t len = sizeof(suSrcAddr);
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
    if (prEnt == NULL || flag || iIsStale) {
        //send RREQ
        RREQ_t RREQ;
        makeRREQ(&RREQ, destIP, ulBroadID++);
        void* bufRREQ = malloc(RREQ_SIZE);
        marshalRREQ(bufRREQ, &RREQ);
        unsigned char localMac[MAC_LEN];
        getLocalVmMac(localMac);
#ifdef DEBUG
        prtMac("local mac", localMac);
#endif
        //create socket
        unsigned char destMac[MAC_LEN] = ODR_BROADCAST_MAC;
        addToRTab(pRouteTab, RREQ.srcIP, localMac, ETH_IF_INDEX, 0);
        sendRawFrame(iRawSock, destMac, localMac, bufRREQ);
        free(bufRREQ);
    } else {//TODO
        //send DATA
    }
}

void onRecvRREQ(RREQ_t* pRREQ, const struct sockaddr_ll *srcAddr) {
    // add "reverse" route to route table
    RTabEnt_t *ent = getRTabEntByDest(pRouteTab, pRREQ->srcIP);
    if (ent == NULL) {
        addToRTab(pRouteTab, pRREQ->srcIP, srcAddr->sll_addr, 
                srcAddr->sll_ifindex, pRREQ->hopCnt + 1);
    } else {
        if (pRREQ->hopCnt + 1 > ent->distToDest
                && strcmp(srcAddr->sll_addr, ent->nextNode) == 0
                && srcAddr->sll_ifindex == ent->outIfIndex ) {
            return;
        }
        updateRTabEnt(ent, srcAddr->sll_addr, srcAddr->sll_ifindex, pRREQ->hopCnt + 1);
    }


    // relay RREQ or send back RREP
    char pcLocalIP[IP_LEN];
    getLocalVmIP(pcLocalIP);
    if (memcmp(pcLocalIP, pRREQ->destIP, MAC_LEN) == 0) {
        //RREP
        RREP_t RREP;
        makeRREP(&RREP, pRREQ, 0);
        void* bufRREP = malloc(RREP_SIZE);
        marshalRREP(bufRREP, &RREP);
        RTabEnt_t *ent = getRTabEntByDest(pRouteTab, RREP.srcIP);
        unsigned char localMac[MAC_LEN];
        getLocalVmMac(localMac);
        sendRawFrame(iRawSock, ent->nextNode, localMac, bufRREP);
    }
    ent = getRTabEntByDest(pRouteTab, pRREQ->destIP);
    if (ent == NULL) {
        //relay RREQ
        void* bufRREQ = malloc(RREQ_SIZE);
        incHopCnt(pRREQ);
        marshalRREQ(bufRREQ, pRREQ);
        unsigned char localMac[MAC_LEN];
        getLocalVmMac(localMac);
        unsigned char destMac[MAC_LEN] = ODR_BROADCAST_MAC;
        sendRawFrame(iRawSock, destMac, localMac, bufRREQ);
    } else {
        //RREP
        RREP_t RREP;
        makeRREP(&RREP, pRREQ, ent->distToDest);
        void* bufRREP = malloc(RREP_SIZE);
        marshalRREP(bufRREP, &RREP);
        RTabEnt_t *e = getRTabEntByDest(pRouteTab, RREP.srcIP);
        unsigned char localMac[MAC_LEN];
        getLocalVmMac(localMac);
        sendRawFrame(iRawSock, e->nextNode, localMac, bufRREP);
    }
}

int sendRawFrame(const int iSockfd, const unsigned char* destAddr, 
        const unsigned char* srcAddr, const void* data) {
    struct sockaddr_ll sockAddr;
    void* buffer = malloc(ETH_FRAME_LEN);
    unsigned char* etherhead = buffer;
    void* usrData = buffer + ETH_DATA_OFFSET;
    struct ethhdr *eh = (struct ethhdr *)etherhead;;
    unsigned char srcMac[MAC_LEN], destMac[MAC_LEN];
    memcpy(srcMac, srcAddr, MAC_LEN);
    memcpy(destMac, destAddr, MAC_LEN);
#ifdef DEBUG
    prtMac("srcMac", srcMac);
    prtMac("destMac", destMac);
#endif

    /*prepare sockaddr_ll*/
    sockAddr.sll_family   = PF_PACKET;	
    sockAddr.sll_protocol = htons(ETH_PROT_VALUE);
    sockAddr.sll_ifindex  = ETH_IF_INDEX;
    sockAddr.sll_hatype   = ARPHRD_ETHER;
    sockAddr.sll_pkttype  = PACKET_OTHERHOST;
    sockAddr.sll_halen    = ETH_ALEN;		
    /*MAC - begin*/
    memcpy(sockAddr.sll_addr, destMac, MAC_LEN);
    sockAddr.sll_addr[6]  = 0x00;/*not used*/
    sockAddr.sll_addr[7]  = 0x00;/*not used*/

    /*set the frame header*/
    memcpy((void*)buffer, (void*)destMac, ETH_ALEN);
    memcpy((void*)(buffer+ETH_ALEN), (void*)srcMac, ETH_ALEN);
    //eh->h_proto = 0x00;
    eh->h_proto = htons(ETH_PROT_VALUE);
    /*fill the frame with some data*/
    memcpy(usrData, data, ETH_DATA_LEN);

    /*send the packet*/
    int n = sendto(iSockfd, buffer, ETH_FRAME_LEN, 0, 
            (SA*)&sockAddr, sizeof(sockAddr));
    if (n == -1) {
        prtErr(ERR_SEND_RAW_DATA);
    }
#ifdef DEBUG
    printf("sent %d bytes.\n", n);
    fflush(stdout);
#endif
    return n;
}

/*
void setMacAddr(unsigned char* target, unsigned char* src) {
    target[0] = hexStr2UChar(strtok(src, ":"));
    for (int i = 1; i < 5; ++i) {
        target[i] = hexStr2UChar(strtok(NULL, ":"));
    }
    target[5] = hexStr2UChar(&src[15]);
}
*/
