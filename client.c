#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "utility.h"
#include "unp.h"
#include "api.h"

int main(int argc, char** argv) {
    int iLocalIndex = getVmIndex();
#ifdef DEBUG
    prtItemInt("local index", iLocalIndex);
#endif

    // create domain datagram socket
	int iSockfd;
	struct sockaddr_un suCliaddr, suServaddr;

	iSockfd = Socket(AF_LOCAL, SOCK_DGRAM, 0);
	char pcFile[] = "template.XXXXXX";

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

	char message[2];
	char src_ip[IP_LEN];
	char dest_ip[IP_LEN];
	int src_port;
	int dest_port;

    while (1) {
    	// step 1
        int iVmNum = 0;
        printf("Please select a VM as server by typing number 1-10, ");
        printf("or type 0 to exit\n");
        scanf("%d", &iVmNum);
        if (iVmNum == 0) {
	        unlink(pcFile);
            exit(0);
        } else if (iVmNum >= 1 && iVmNum <= 10) {
            printf("Your selection is VM %d\n\n", iVmNum);
        }
    }
}

