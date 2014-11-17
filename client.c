#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    // create domain datagram socket
	int sockfd;
	struct sockaddr_un cliaddr servaddr;

	sockfd=Socket(AF_LOCAL, SOCK_DGRAM, 0);
	char file[]="template.XXXXXX";
	// create a temp file
	int tmpFile=mkstemp(file);
	if(tmpFile==-1) {
		printf("create temp file failed /n");
		exit(1);
	}

	unlink(file);

	bzero(&cliaddr, sizeof(cliaddr) );          //  bind an address for us
	cliaddr.sun_family=AF_LOCAL;
	strcpy(cliaddr.sun_path, file);

	Bind(sockfd, (SA*) &cliaddr, sizeof(cliaddr) );


    while (1) {
        int iVmNum = 0;
        printf("Please select a VM by typing number 1-10, ");
        printf("or type 0 to exit\n");
        scanf("%d", &iVmNum);
        if (iVmNum == 0) {
            exit(0);
        } else if (iVmNum >= 1 && iVmNum <= 10) {
            printf("Your selection is VM %d\n\n", iVmNum);
        }
    }
}

