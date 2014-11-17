#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include "lib/hw_addrs.h"
#include "constants.h"
#include "unp.h"

void packAppData(int sockfd, char* destIP, int destPort, char* msg, int flag) {
    //TODO : network byte order?

}

int getVmIndex() {
    struct hwa_info *hwa, *hwaHead;
    char* ipArray[VM_NUM] = {IP01, IP02, IP03, IP04, IP05, IP06, IP07, IP08, IP09, IP10};

    hwa = hwaHead = Get_hw_addrs();
    for (; hwa != NULL; hwa = hwa->hwa_next) {
        struct sockaddr *sa = hwa->ip_addr;
        char* pcIfName = hwa->if_name;
        //get eth0 address
        if (strcmp(pcIfName, "eth0") == 0) {
            char* pcAddr = Sock_ntop_host(sa, sizeof(*sa));
            for (int i = 0; i < VM_NUM; ++i) {
                if (strcmp(ipArray[i], pcAddr) == 0) {
                    return i + 1;
                }
            }
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
