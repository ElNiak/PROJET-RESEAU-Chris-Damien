#include <stdlib.h>
#include <stdio.h>
#include "real_address.h"
#include "create_socket.h"
#include "wait_for_client.h"
#include "read_write_loop.h"
#include "sender.h"
#include "various.h"
#include <netinet/in.h>

char *file=NULL;
char *hostname=NULL;
int port=-1;

int main(int argc, char **argv){
	get_args(argc,argv);

	struct sockaddr_in6 addr;
	int err=real_address(hostname,&addr);
	if(err){
		fprintf(stderr,"could not resolve hostname %s, %s\n",hostname, err);	
		return -1;
	}
	
	int sfd = create_socket(NULL,&addr);
	if(sfd == -1){
		fprintf(stderr, "could not create socket : sfd = -1\n");
		return -1;
	}
	int fd;
	if(file==NULL)
	{
		fd=1	
	}else{
		fd=open(file,O_WRONLY|O_CREATE);
	}
}

int sender_selective_repeat(int sockfd, int fd)
{
	struct pollfd pfds[2];
	int nbFd;
	int loop=1;
	while(loop){
		pfds[0].fd = sockfd;
    		pfds[0].events = POLLIN | POLLPRI;

    		pfds[1].fd = sockfd;
    		pfds[1].events = POLLIN | POLLPRI;

    		nbFd = poll(pfds,2,-1); //timeout = -1 => Pour illimite
    		if(nbFd == -1)
    		{
      			fprintf(stderr, "error poll()");
      			return -1;
    		}
	}
}
