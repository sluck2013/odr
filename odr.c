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
unsigned int ulBroadID;
unsigned long int staleness;
int iDomSock = 0, iRawSock = 0;
IfInfo_t arrIfInfo[6];
int iIfNum;
AppMsg_t appMsgSend;

int main(int argc, char** argv) {
    if (argc != 2) {
        char msg[APP_DATA_LEN];
        sprintf(msg, "Usage: %s <staleness>", argv[0]);
        errExit(msg);
    }
    iIfNum = getLocalIfInfo(arrIfInfo);
#ifdef DEBUG
    for (int i = 0; i < iIfNum; ++i) {
        prtItemInt("interface index", arrIfInfo[i].index);
        prtItemStr("interface name", arrIfInfo[i].name);
        prtMac("interface mac", arrIfInfo[i].mac);
        printf("\n");
    }
#endif

    ulBroadID = 0;
    staleness = strtoul(argv[1], NULL, 10);
#ifdef DEBUG
    printf("staleness: %lu\n", staleness);
#endif

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
            onDomSockAvailable();
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
    unsigned char iMsgType;
    memcpy((void*)&iMsgType, buffer + ETH_DATA_OFFSET, sizeof(iMsgType));
    switch (iMsgType) {
        case 0:
            ; // don't remove it
            RREQ_t RREQ;
            unmarshalRREQ(&RREQ, buffer + ETH_DATA_OFFSET);
            free(buffer);
            onRecvRREQ(&RREQ, &srcAddr);
            break;
        case 1:
            ; // don't remove it
            RREP_t RREP;
            unmarshalRREP(&RREP, buffer + ETH_DATA_OFFSET);
            free(buffer);
            onRecvRREP(&RREP, &srcAddr);
            break;
        case 2:
            ; // don't remove it
            AppMsg_t appMsgRecv;
            unmarshalAppMsg(&appMsgRecv, buffer + ETH_DATA_OFFSET);
            free(buffer);
            onRecvAppMsg(&appMsgRecv, &srcAddr);
            break;
        default:
            break;
    }
}

void onDomSockAvailable() {
    char pcReadBuf[MAXLINE + 1];
    char destIP[IP_LEN], srcIP[IP_LEN];
    int srcPort, destPort;
    unsigned char flag;
    char msg[2];
    struct sockaddr_un suSrcAddr;
    socklen_t len = sizeof(suSrcAddr);
    int n = recvfrom(iDomSock, pcReadBuf, MAXLINE, 0, (SA*)&suSrcAddr, &len);
#ifdef DEBUG
    printf("Received %d bytes from application.\n", n);
#endif
    
    deleteExpiredPTabEnts(pPathTab);
    PTabEnt_t *pEnt = findPTabEntByPath(pPathTab, suSrcAddr.sun_path);
    if (pEnt == NULL) {
        srcPort = getNewPTabPort(pPathTab);
        addToPathTable(pPathTab, srcPort, 
                suSrcAddr.sun_path, PTAB_ENT_LIFETIME);
    } else {
        srcPort = pEnt->port;
        confirmPTabEnt(pEnt);
    }

    getLocalVmIP(srcIP);
    unpackAppData(pcReadBuf, destIP, &destPort, msg, &flag);
    makeAppMsg(&appMsgSend, srcPort, destPort, srcIP, destIP, msg);
#ifdef DEBUG
    printf("dest: %s:%u  forceDiscover:%d\n\n", destIP, destPort, flag);
    fflush(stdout);
#endif
    if (flag) {
        deleteRTabEntsToDest(pRouteTab, destIP);
    }
    deleteStaleRTabEnts(pRouteTab, staleness);
    RTabEnt_t *prEnt = getRTabEntByDest(pRouteTab, destIP);
    if (prEnt == NULL) {
        //flood RREQ
        RREQ_t RREQ;
        makeRREQ(&RREQ, destIP, bid(), flag);
        //add itself to rtab to avoid loopback
        //last 3 params not in use
        //addToRTab(pRouteTab, RREQ.srcIP, arrIfInfo[0].mac, 0, 0);
        floodRREQ(iRawSock, -1, &RREQ, 1);
    } else {
        sendAppMsg(&appMsgSend);
    }
}

void onRecvRREQ(RREQ_t* pRREQ, const struct sockaddr_ll *srcAddr) {
    const int iInIndex = srcAddr->sll_ifindex;
    const unsigned char* pucSrcMac = srcAddr->sll_addr;
#ifdef DEBUG
    prtMac("Received RREQ from", pucSrcMac);
    prtItemInt("Incoming ifIndex", iInIndex);
    prtRREQ(pRREQ);
    prtln();
#endif
    // add "reverse" route to route table
    if (pRREQ->forceDisc) {
        deleteRTabEntsToDest(pRouteTab, pRREQ->srcIP);
        deleteRTabEntsToDest(pRouteTab, pRREQ->destIP);
    }
    deleteStaleRTabEnts(pRouteTab, staleness);
    RTabEnt_t *ent = getRTabEntByDest(pRouteTab, pRREQ->srcIP);
    if (ent == NULL) {
        addToRTab(pRouteTab, pRREQ->srcIP, pucSrcMac, iInIndex, pRREQ->hopCnt + 1);
    } else {
        if (isRTabEntEqual(ent, pucSrcMac, iInIndex, pRREQ->hopCnt + 1)) {
            confirmRTabEnt(ent);
        } else if (pRREQ->hopCnt + 1 < ent->distToDest
                || (pRREQ->hopCnt + 1 == ent->distToDest
                    && memcmp(pucSrcMac, ent->nextNode, MAC_LEN) != 0
                   )
                ) {
            updateRTabEnt(ent, pucSrcMac, iInIndex, pRREQ->hopCnt + 1);
        } else {
            return;
        }
    }

    // relay RREQ or send back RREP
    char pcLocalIP[IP_LEN];
    getLocalVmIP(pcLocalIP);
    if (strcmp(pcLocalIP, pRREQ->destIP) == 0) {
#ifdef DEBUG
        prtMsg("Reached destination");
#endif
        //RREP
        if (isResponsed(pRREQ)) {
            return;
        }
        RREP_t RREP;
        makeRREP(&RREP, pRREQ, 0);
        void* bufRREP = malloc(RREP_SIZE);
        marshalRREP(bufRREP, &RREP);
        RTabEnt_t *ent = getRTabEntByDest(pRouteTab, RREP.srcIP);
        int ifidx = ent->outIfIndex;
        int aridx = getArrIdxByIfIdx(ifidx);
        sendRawFrame(iRawSock, ent->nextNode, arrIfInfo[aridx].mac, ifidx, bufRREP);
#ifdef DEBUG
        prtMac("Sent RREP to", ent->nextNode);
        prtItemInt("Out ifindex", ifidx);
        prtRREP(&RREP);
        prtln();
#endif
    } else {
        ent = getRTabEntByDest(pRouteTab, pRREQ->destIP);
        if (ent == NULL) {
#ifdef DEBUG
            prtMsg("Reached intermediate node");
#endif
            //relay RREQ
            incRREQHopCnt(pRREQ);
            floodRREQ(iRawSock, iInIndex, pRREQ, 0);
        } else {
#ifdef DEBUG
            prtMsg("Current node has destination routing info");
#endif
            //RREP
            if (!isResponsed(pRREQ)) {
                RREP_t RREP;
                makeRREP(&RREP, pRREQ, ent->distToDest);
                void* bufRREP = malloc(RREP_SIZE);
                marshalRREP(bufRREP, &RREP);
                RTabEnt_t *e = getRTabEntByDest(pRouteTab, RREP.srcIP);
                int ifidx = e->outIfIndex;
                int aridx = getArrIdxByIfIdx(ifidx);
                sendRawFrame(iRawSock, e->nextNode, arrIfInfo[aridx].mac, ifidx, bufRREP);
#ifdef DEBUG
                prtMac("Sent RREP to", e->nextNode);
                prtItemInt("Out ifindex", ifidx);
                prtRREP(&RREP);
                prtln();
#endif
                setRespBit(pRREQ);
            }
            incRREQHopCnt(pRREQ);
            floodRREQ(iRawSock, iInIndex, pRREQ, 0);
            
        }
    }
}

void onRecvRREP(RREP_t* pRREP, const struct sockaddr_ll *incomingAddr) {
    const int iInIndex = incomingAddr->sll_ifindex;
    const unsigned char* pucSrcMac = incomingAddr->sll_addr;
    if (pRREP->forceDisc) {
        deleteRTabEntsToDest(pRouteTab, pRREP->destIP);
    }
    RTabEnt_t* ent = getRTabEntByDest(pRouteTab, pRREP->destIP);
    if (ent == NULL) {
        addToRTab(pRouteTab, pRREP->destIP, pucSrcMac, iInIndex, pRREP->hopCnt + 1);
    } else {
        if (isRTabEntEqual(ent, pucSrcMac, iInIndex, pRREP->hopCnt + 1)) {
        } else if (pRREP->hopCnt + 1 < ent->distToDest) {
            updateRTabEnt(ent, pucSrcMac, iInIndex, pRREP->hopCnt + 1);
        } else {
            return;
        }
    }

    char localIP[IP_LEN];
    getLocalVmIP(localIP);
    if (strcmp(localIP, pRREP->srcIP) == 0) {
#ifdef DEBUG
        prtMsg("RREP reached src node");
#endif
        sendAppMsg(&appMsgSend);
    } else {
#ifdef DEBUG
        prtMsg("RREP reached intermediate node");
#endif
        RTabEnt_t* eSrc = getRTabEntByDest(pRouteTab, pRREP->srcIP);
        int ifidx = eSrc->outIfIndex;
        int aridx = getArrIdxByIfIdx(ifidx);
        incRREPHopCnt(pRREP);
        void *bufRREP = malloc(RREP_SIZE);
        marshalRREP(bufRREP, pRREP);
        sendRawFrame(iRawSock, eSrc->nextNode, arrIfInfo[aridx].mac, ifidx, bufRREP);
        free(bufRREP);
#ifdef DEBUG
        prtMac("Sent RREP to", eSrc->nextNode);
        prtRREP(pRREP);
        prtItemInt("Out ifindex", ifidx);
        prtln();
#endif
    }
}

void onRecvAppMsg(AppMsg_t *appMsgRecv, const struct sockaddr_ll* srcAddr) {
    const int iInIndex = srcAddr->sll_ifindex;
    const unsigned char* pucSrcMac = srcAddr->sll_addr;
#ifdef DEBUG
    printf("Received app payload:\n");
    prtAppMsg(appMsgRecv);
    prtMac("from", pucSrcMac);
    prtItemInt("Incoming interface index", iInIndex);
    prtln();
#endif
    // update / confirm routing table
    RTabEnt_t *eSrc = getRTabEntByDest(pRouteTab, appMsgRecv->srcIP);
    if (eSrc == NULL) {
        addToRTab(pRouteTab, appMsgRecv->srcIP, pucSrcMac, 
                iInIndex, appMsgRecv->hopCnt + 1);
    } else {
        if (appMsgRecv->hopCnt + 1 < eSrc->distToDest) {
            updateRTabEnt(eSrc, pucSrcMac, 
                    iInIndex, appMsgRecv->hopCnt + 1);
        } else if (isRTabEntEqual(eSrc, pucSrcMac,
                    iInIndex, appMsgRecv->hopCnt + 1)) {
            confirmRTabEnt(eSrc);
        }
    }

    // relay or send to client/server
    char localIP[IP_LEN];
    getLocalVmIP(localIP);
    if (strcmp(localIP, appMsgRecv->destIP) == 0) {
        //relay to client / server   
        deleteExpiredPTabEnts(pPathTab);
        PTabEnt_t* pEnt = findPTabEntByPort(pPathTab, appMsgRecv->destPort);
        if (pEnt == NULL) {
            prtErr(ERR_GET_PATH_BY_PORT);
        } else {
            confirmPTabEnt(pEnt);
            char data[MAXLINE];
            packAppData(data, appMsgRecv->srcIP, appMsgRecv->srcPort, appMsgRecv->msg, 0); // last param not in use
            struct sockaddr_un suAppAddr;
            bzero(&suAppAddr, sizeof(suAppAddr));
            suAppAddr.sun_family = AF_LOCAL;
            strcpy(suAppAddr.sun_path, pEnt->sunPath);
            Sendto(iDomSock, data, sizeof(data), 0, (SA*)&suAppAddr, sizeof(suAppAddr));
        }
        return;
    }

    RTabEnt_t *eDest = getRTabEntByDest(pRouteTab, appMsgRecv->destIP);
    incAppMsgHopCnt(appMsgRecv);
    sendAppMsg(appMsgRecv);
}

void floodRREQ(const int iSockfd, const int incomeIfIdx, RREQ_t *pRREQ, const int isSrc) {
    unsigned char destMac[MAC_LEN] = ODR_BROADCAST_MAC;
    for (int i = 0; i < iIfNum; ++i) {
        if (arrIfInfo[i].index == incomeIfIdx) {
            continue;
        }
        if (isSrc) {
            setBroadID(pRREQ, bid());
        }
        void* bufRREQ = malloc(RREQ_SIZE);
        marshalRREQ(bufRREQ, pRREQ);
        sendRawFrame(iSockfd, destMac, arrIfInfo[i].mac, arrIfInfo[i].index, bufRREQ);
#ifdef DEBUG
        prtMac("Sent RREQ to", destMac);
        prtItemInt("Out ifIndex", arrIfInfo[i].index);
        prtRREQ(pRREQ);
        prtln();
#endif
        free(bufRREQ);
    }
}

void sendAppMsg(const AppMsg_t *appMsg) {
    void *buffer = malloc(APPMSG_SIZE);
    marshalAppMsg(buffer, appMsg);
    RTabEnt_t *ent = getRTabEntByDest(pRouteTab, appMsg->destIP);
    if (ent == NULL) {
        prtErr(ERR_ROUTE_NOT_EXIST);
    }
    int outIdx = ent->outIfIndex;
    int arrIdx = getArrIdxByIfIdx(outIdx);
    sendRawFrame(iRawSock, ent->nextNode, arrIfInfo[arrIdx].mac, outIdx, buffer);
    free(buffer);
#ifdef DEBUG
    prtMac("Sent app payload to", ent->nextNode);
    prtItemInt("out interface index", outIdx);
    prtAppMsg(appMsg);
    prtln();
#endif
}

int sendRawFrame(const int iSockfd, const unsigned char* destAddr, 
        const unsigned char* srcAddr, const int ifIndex, const void* data) {
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
#endif

    /*prepare sockaddr_ll*/
    sockAddr.sll_family   = PF_PACKET;	
    sockAddr.sll_protocol = htons(ETH_PROT_VALUE);
    sockAddr.sll_ifindex  = ifIndex;
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
    } else {
#ifdef RELEASE
        int idx = getLocalVmIndex();
        unsigned char msgType;
        char srcIP[IP_LEN], destIP[IP_LEN];
        char nextMac[20];
        memcpy((void*)&msgType, data, 1);
        memcpy((void*)srcIP, data + 1, IP_LEN);
        memcpy((void*)destIP, data + 1 + IP_LEN, IP_LEN);
        printf("ODR at node vm %d: sending frame hdr src vm %d  dest %s",
                idx, idx, macToString(nextMac, destMac));
        printf("                           ODR msg type %u  src vm %d  dest vm %d\n",
                msgType, getVmIndexByIP(srcIP), getVmIndexByIP(destIP));
        prtln();
#endif
    }
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
int getArrIdxByIfIdx(const int ifIndex) {
    for (int i = 0; i < iIfNum; ++i) {
        if (arrIfInfo[i].index == ifIndex) {
            return i;
        }
    }
    return -1;
}

unsigned int bid() {
    return ulBroadID++;
}
