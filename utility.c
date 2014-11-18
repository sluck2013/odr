#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include "lib/hw_addrs.h"
#include "constants.h"
#include "unp.h"

void packAppData(char* data,const char* destIP, const int destPort, const char* msg, const int flag) {
    data[0]='\0';   
    char ch[]="/";
    char buffer1[20];
    char buffer2[20];

    strcat(data, destIP);
    strcat(data,ch);

    sprintf(buffer1, "%d", destPort);
    strcat(data, buffer1);
    strcat(data, ch);

    strcat(data, msg);
    strcat(data,ch);

    sprintf(buffer2, "%d", flag);
    strcat(data, buffer2);
    strcat(data,ch);
}

void unpackAppData(const char* data, char* srcIP, int* srcPort, char* msg) {

}

char* getVmIP(char* ip, const int index) {
    char* ipArray[VM_NUM] = {IP01, IP02, IP03, IP04, IP05, IP06, IP07, IP08, IP09, IP10};
    return strcpy(ip, ipArray[index - 1]);
}

int getVmIndex() {
    struct hwa_info *hwa, *hwaHead;
//TODO: use gethostbyname
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
    char* ipArray[VM_NUM] = {IP01, IP02, IP03, IP04, IP05, IP06, IP07, IP08, IP09, IP10};
    for (int i = 0; i < VM_NUM; ++i) {
        if (strcmp(ipArray[i], IP) == 0) {
            return i + 1;
        }
    }
    return -1;
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

void prtItemString(const char *key, const char* value) {
    printf("%s: %s\n", key, value);
}
