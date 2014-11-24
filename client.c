#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "utility.h"
#include "unp.h"
#include "time.h"
#include "api.h"

int main(int argc, char** argv) {
    int iLocalIndex = getLocalVmIndex();
#ifdef DEBUG
    prtItemInt("local index", iLocalIndex);
#endif

    // create domain datagram socket
	int iSockfd;
	struct sockaddr_un suCliaddr, suServaddr;

	iSockfd = Socket(AF_LOCAL, SOCK_DGRAM, 0);
	char pcFile[] = "109399621.XXXXXX";

	// create temp file
	int iTmpFile = mkstemp(pcFile);
	if(iTmpFile == -1) {
        errExit(ERR_CREATE_TEMP_SOCK_FILE);
	}

	unlink(pcFile);

	bzero(&suCliaddr, sizeof(suCliaddr));
	suCliaddr.sun_family = AF_LOCAL;
	strcpy(suCliaddr.sun_path, pcFile);

	Bind(iSockfd, (SA*) &suCliaddr, sizeof(suCliaddr));

    while (1) {
        int iVmNum = 0;
        printf("\nPlease select a VM as server by typing number 1-10, ");
        printf("or type 0 to exit\n");
        scanf("%d", &iVmNum);
        if (iVmNum == 0) {
	        unlink(pcFile);
            exit(0);
        } else if (iVmNum >= 1 && iVmNum <= 10) {
            printf("Your selection is VM %d\n", iVmNum);
            char destVmIP[IP_LEN];
            char msg[] = "1";
            int destPort = SERV_WK_PORT;
            getVmIPByIndex(destVmIP, iVmNum);
#ifdef DEBUG
            prtItemStr("Destination VM IP", destVmIP);
#endif
            //TODO: timeout 542 601
            //try pselect and sigalarm
            msg_send(iSockfd, destVmIP, destPort, msg, 0);
            printf("client at node vm %d sending request to server at vm %d\n", iLocalIndex, iVmNum);

            msg_recv(iSockfd, msg, destVmIP, &destPort);
            printf("client at node vm %d received from vm %d %lu\n", iLocalIndex, getVmIndexByIP(destVmIP), time(NULL));
        }
    }
}

