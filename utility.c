#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include "lib/hw_addrs.h"
#include "constants.h"
#include "unp.h"
#include "ctype.h"

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

    strcat(data, ch);

    return data;
}

void unpackAppData(char* data, char* IP, int* port, char* msg, int* flag) {
    char ch[] = "/";
    strcpy(IP, strtok(data, ch));
    *port = atoi(strtok(NULL, ch));
    strcpy(msg,  strtok(NULL, ch));
    if (flag != NULL) {
        *flag = atoi(strtok(NULL, ch));
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
    char localIP[IP_LEN];
    getLocalVmIP(localIP);
    return getVmIndexByIP(localIP);
}

void getLocalVmIP(char* localIP) {
    struct hwa_info *hwa, *hwaHead;
    hwa = hwaHead = Get_hw_addrs();
    for (; hwa != NULL; hwa = hwa->hwa_next) {
        struct sockaddr *sa = hwa->ip_addr;
        char* pcIfName = hwa->if_name;
        //get eth0 address
        if (strcmp(pcIfName, ODR_IF_NAME) == 0) {
            strcpy(localIP, Sock_ntop_host(sa, sizeof(*sa)));
            return;
        }
    }
    localIP[0] = '\0';
}

void getLocalVmMac(char* localMac) {
    struct hwa_info *hwa, *hwaHead;
    hwa = hwaHead = Get_hw_addrs();
    for (; hwa != NULL; hwa = hwa->hwa_next) {
        struct sockaddr *sa = hwa->ip_addr;
        char* pcIfName = hwa->if_name;
        //get eth0 address
        if (strcmp(pcIfName, ODR_IF_NAME) == 0) {
            char* ptr = hwa->if_haddr;
            int i = IF_HADDR;
            char* wrPtr = localMac;
            do {
                sprintf(wrPtr, "%.2x%s", *ptr++ & 0xff, (i == 1) ? "" : ":");
                wrPtr += 3;
            } while (--i > 0);
            return;
        }
    }
    localMac[0] = '\0';
}

int getVmIndexByIP(const char* IP) {
    struct in_addr iaIP;
    inet_pton(AF_INET, IP, &iaIP);
    struct hostent *h = gethostbyaddr(&iaIP, sizeof(iaIP), AF_INET);
#ifdef LDEBUG
    return -1;
#else
    return atoi(&h->h_name[2]);
#endif
}

unsigned char hexStr2UChar(char const* str) {
    unsigned char r = 0;
    for (int i = 0; i < 2; ++i) {
        r <<= 4;
        char c = toupper(*(str + i));

        if (c >= ASCII_0 && c <= ASCII_9) {
            r += c - ASCII_0;
        } else if (c >= ASCII_A && c <= ASCII_F) {
            r += c - ASCII_A + 10;
        }
    }
    return r;
}

void prtErr(const char *msg) {
    printf("ERROR: %s\n", msg);
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

#ifdef DEBUG
void prtMac(const char* title, const unsigned char* mac) {
    printf("%s: ", title);
    for (int i = 0; i < 5; ++i) {
        printf("%x:", mac[i]);
    }
    printf("%x\n", mac[5]);
}
#endif
