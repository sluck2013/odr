#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include "lib/hw_addrs.h"
#include "constants.h"
#include "unp.h"
#include "ctype.h"

char* packAppData(char* data, const char* IP, const unsigned short int port, const char* msg, const unsigned char flag) {
    data[0] = '\0';   
    char ch[] = "/";
    char buffer[20];

    strcat(data, IP);
    strcat(data, ch);    

    sprintf(buffer, "%u", port);
    strcat(data, buffer);
    strcat(data, ch);

    strcat(data, msg);
    strcat(data, ch);

    sprintf(buffer, "%u", flag);
    strcat(data, buffer);

    strcat(data, ch);

    return data;
}

void unpackAppData(char* data, char* IP, unsigned short int* port, char* msg, unsigned char* flag) {
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
        if (strcmp(pcIfName, "eth0") == 0) {
            strcpy(localIP, Sock_ntop_host(sa, sizeof(*sa)));
            free_hwa_info(hwaHead);
            return;
        }
    }
    localIP[0] = '\0';
    free_hwa_info(hwaHead);
}

int getLocalIfInfo(IfInfo_t* localIf) {
    struct hwa_info *hwa, *hwaHead;
    hwa = hwaHead = Get_hw_addrs();
    int ctr = 0;
    for (; hwa != NULL; hwa = hwa->hwa_next) {
        struct sockaddr *sa = hwa->ip_addr;
        char* pcIfName = hwa->if_name;
        //get eth0 address
        if (strcmp(pcIfName, "eth0") != 0
                && strcmp(pcIfName, "lo") != 0) {
            (localIf + ctr)->index = hwa->if_index;
            strcpy((localIf + ctr)->name, hwa->if_name);
            memcpy((localIf + ctr)->mac, hwa->if_haddr, MAC_LEN);
            ++ctr;
        }
    }
    free_hwa_info(hwaHead);
    return ctr;
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

        if (c >= '0' && c <= '9') {
            r += c - '0';
        } else if (c >= 'A' && c <= 'F') {
            r += c - 'A' + 10;
        }
    }
    return r;
}

void prtErr(const char *msg) {
    printf("ERROR: %s\n", msg);
    /*if (errno != 0) {
        printf("DETAILS: %s\n", strerr(errno));
    }*/
    fflush(stdout);
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

void prtMsg(const char *key) {
    printf("%s\n", key);
    fflush(stdout);
}

void prtln() {
    printf("\n");
    fflush(stdout);
}
#ifdef DEBUG
void prtMac(const char* title, const unsigned char* mac) {
    printf("%s: ", title);
    for (int i = 0; i < 5; ++i) {
        printf("%.2x:", mac[i]);
    }
    printf("%.2x\n", mac[5]);
}
#endif
