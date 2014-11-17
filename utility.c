#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

void packAppData(int sockfd, char* destIP, int destPort, char* msg, int flag) {
    //TODO : network byte order?

}

void prtErr(char *msg) {
    printf("%s\n", msg);
}

void errExit(char *msg) {
    prtErr(msg);
    exit(1);
} 
