#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include "lib/hw_addrs.h"
#include "constants.h"
#include "unp.h"

char* packAppData(char* data,const char* IP, const int port, const char* msg, const int flag) {
    data[0] = '\0';   
    char ch[] = "/";
    char buffer[20];

    strcat(data, IP);
    strcat(data, ch);    

    sprintf(buffer, "%d", port);
    strcat(data, buffer);
    strcat(data, ch);

    strcat(data, msg);
    strcat(data, ch);

    sprintf(buffer, "%d", flag);
    strcat(data, buffer);

    return data;
}

void unpackAppData(char* data, char* IP, int* port, char* msg, int* flag) {
    char ch[] = "/";
    strcpy(IP, strtok(data, ch));
    *port = atoi(strtok(NULL, ch));
    strcpy(msg,  strtok(NULL, ch));
    if (flag != NULL) {
        *flag = atoi(data);
    }
}

char* getVmIPByIndex(char* ip, const int index) {
    char hostName[5];
    char ipBuf[16];
    sprintf(hostName, "vm%d", index);
    struct hostent *h = gethostbyname(hostName);
    inet_ntop(h->h_addrtype, h->h_addr, ipBuf, sizeof(ipBuf));
    strcpy(ip, ipBuf);
}

int getLocalVmIndex() {
    struct hwa_info *hwa, *hwaHead;
    hwa = hwaHead = Get_hw_addrs();
    for (; hwa != NULL; hwa = hwa->hwa_next) {
        struct sockaddr *sa = hwa->ip_addr;
        char* pcIfName = hwa->if_name;
        //get eth0 address
        if (strcmp(pcIfName, "eth0") == 0) {
            char* pcAddr = Sock_ntop_host(sa, sizeof(*sa));
            return getVmIndexByIP(pcAddr);
        }
    }
    return -1;
}

int getVmIndexByIP(const char* IP) {
    struct in_addr iaIP;
    inet_pton(AF_INET, IP, &iaIP);
    struct hostent *h = gethostbyaddr(&iaIP, sizeof(iaIP), AF_INET);
    return atoi(&h->h_name[2]);
}

void prtErr(const char *msg) {
    printf("%s\n", msg);
}

void errExit(const char *msg) {
    prtErr(msg);
    exit(1);
} 

void prtItemInt(const char *key, const int value) {
    printf("%s: %d\n", key, value);
}

void prtItemStr(const char *key, const char* value) {
    printf("%s: %s\n", key, value);
}
