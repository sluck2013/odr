#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "utility.h"
#include "unp.h"
#include "api.h"

int main(int argc, char** argv) {
	// create a domain datagram socket
	int iSockfd;
	struct sockaddr_un suServaddr;

	iSockfd = Socket(AF_LOCAL, SOCK_DGRAM, 0);

	unlink(SERV_WK_PATH);

	bzero(&suServaddr, sizeof(suServaddr));
	suServaddr.sun_family = AF_LOCAL;
	strcpy(suServaddr.sun_path, SERV_WK_PATH);

	Bind(iSockfd, (SA *) &suServaddr, sizeof(suServaddr) );

	char pcMsg[2];
	char pcSrcIP[IP_LEN];
	int iSrcPort;

    while(1) {
        printf ("waiting for request from client...... \n");  	
        msg_recv(iSockfd, pcMsg, pcSrcIP, &iSrcPort);
        msg_send(iSockfd, pcSrcIP, iSrcPort, pcMsg ,0);
        printf("server at node vm %d responding to request from vm %d\n", getVmIndex(), getVmIndexByIP(pcSrcIP) );
    }
}
