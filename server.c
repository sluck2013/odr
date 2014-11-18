#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "utility.h"
#include "unp.h"
#include "api.h"

int main(int argc, char** argv) {
	// create a domain datagram socket
	int sockfd;
	struct sockaddr_un servaddr;

	sockfd=Socket(AF_LOCAL, SOCK_DGRAM, 0);

	unlink(SERV_WELLKNOWN_PATH);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sun_family=AF_LOCAL;
	strcpy(servaddr.sun_path, SERV_WELLKNOWN_PATH);

	Bind(sockfd, (SA *) &servaddr, sizeof(servaddr) );

	char message[2];
	char src_ip[IP_LEN];
	char dest_ip[IP_LEN];
	int src_port;
	int dest_port;

	while(1) {
	msg_recv(sockfd, message, src_ip, &src_port );
	msg_send(sockfd, dest_ip, &dest_port, message ,0);
	printf("server at node vm%d responding to request from vm%d\n", getVmIndex(src_ip), getVmIndex(dest_ip) );
	}
}