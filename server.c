#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	// create a domain datagram socket
	int sockfd;
	struct sockaddr_un servaddr;

	sockfd=Socket(AF_LOCAL, SOCK_DGRAM, 0);

	unlink(SERV_WKPATH);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sun_family=AF_LOCAL;
	strcpy(servaddr.sun_path, SERV_WKPATH);

	Bind(sockfd, (SA *) &servaddr, sizeof(servaddr) );

	char *message;
	char src_ip[IP_LEN];
	while(1) {
	msg_recv(sockfd, message,  )
	}
}