#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "utility.h"
#include "unp.h"
#include "time.h"
#include "api.h"
#include<setjmp.h>

static sigjmp_buf jmpbuf;
static void timeout(int signo);
int choice=0;
int flag=0;
char timeBuf[50];
int iSockfd;
 char destVmIP[IP_LEN];
 char msg[] = "1";
 int destPort = SERV_WK_PORT;

int main(int argc, char** argv) {
    int iLocalIndex = getVmIndex();
#ifdef DEBUG
    prtItemInt("local index", iLocalIndex);
#endif

    // create domain datagram socket
	struct sockaddr_un suCliaddr, suServaddr;
	time_t ticks;

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

	signal(SIGALRM, timeout);
    while (1) {
        int iVmNum = 0;
        client_request:
        printf("\nPlease select a VM as server by typing number 1-10, ");
        printf("or type 0 to exit\n");
        scanf("%d", &iVmNum);
        if (iVmNum == 0) {
	        unlink(pcFile);
            exit(0);
        } else if (iVmNum >= 1 && iVmNum <= 10) {
            printf("Your selection is VM %d\n", iVmNum);
            //char destVmIP[IP_LEN];
            //char msg[] = "1";
            //int destPort = SERV_WK_PORT;
            getVmIP(destVmIP, iVmNum);
#ifdef DEBUG
            prtItemString("Destination VM IP", destVmIP);
#endif
          
            //TODO: timeout 542 601
            //try pselect and sigalarm

            alarm(50);      // set alarm for 50 seconds
            msg_send(iSockfd, destVmIP, destPort, msg, 0);    // client sending message
            printf("client at node vm %d sending request to server at vm %d\n", iLocalIndex, iVmNum);

            if(sigsetjmp(jmpbuf,1)!=0) {
            	if(choice) {
            		choice=0;
            		goto client_request;
            	}
            	else {
            		choice=1;
            		goto receiving_message;
            	}
            }

            receiving_message:
            printf("waiting for response from server \n");
            msg_recv(iSockfd, msg, destVmIP, &destPort);
            choice=0;
            alarm(0);

            ticks=time(NULL);
            memset(timeBuf,0,sizeof(timeBuf));

            snprintf(timeBuf, sizeof(timeBuf), "%.24s\r\n", ctime(&ticks) );
            //printf("client at node vm %d received from vm %d %lu\n", iLocalIndex, getVmIndexByIP(destVmIP), timeBuf);
            printf("client at node vm %d received from vm %d at : %s\n", iLocalIndex, getVmIndexByIP(destVmIP), timeBuf);
        }
    }

    unlink(pcFile);
    exit(0);
}

static void timeout(int signo) {
	     int iLocalIndex = getVmIndex();
	     //char destVmIP[IP_LEN];
            //char msg[] = "1";
            //int destPort = SERV_WK_PORT;
	if(choice==0) {
		printf("client at node vm%d: timeout on response from vm%d \n", iLocalIndex, getVmIndexByIP(destVmIP) );
		printf("retransmitting message \n");
		alarm(50);
		msg_send(iSockfd, destVmIP, destPort, msg, 1);
	}
	siglongjmp(jmpbuf,1);
	return;
}
