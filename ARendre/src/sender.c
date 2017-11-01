#include "real_address.h"
#include "create_socket.h"
#include "wait_for_client.h"
#include "read_write_loop.h"
#include "sender.h"
#include "packet_interface.h"
#include <stdlib.h>
#include <netinet/in.h>
#include <getopt.h>
#include <poll.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <zlib.h>

char *file=NULL;
char *hostname=NULL;
char *port=NULL;

void get_args(int argc, char **argv){

	int opt;
	if (argc < 2) {
	        fprintf(stderr, "`%s' arguments missing\n", argv[0]);
	        fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
	        exit(1);
	    }
	    else {
	        /* specify which parameters we expect */
	        static struct option options[] =
	        {
	            {"filename", required_argument, 0, 'f'},
	            {"help", no_argument, NULL, 'h'},
	            {NULL, 0, NULL, 0}
	        };

	        while ((opt = getopt_long(argc, argv, "f:h:", options, NULL)) != -1) {
	            switch (opt) {
	                case 'f':
	                    file = optarg;
	                    break;
	                case 'h':
	                    printf("\n");
	                    printf("Usage: %s hostname port\n", argv[0]);
	                    printf("Usage: %s [-f X] hostname port\n", argv[0]);
	                    printf("Usage: %s [--filename X] hostname port\n", argv[0]);
	                    printf("\n");
	                    exit(1);
	                default:
	                    fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
	                    exit(1);
	            }
	        }

	        hostname = argv[argc-2];
	        port = argv[argc-1];
	    }

	}

int main(int argc, char **argv){

	get_args(argc,argv);

	struct sockaddr_in6 addr;
	const char *err=real_address(hostname,&addr);
	if(err){
		fprintf(stderr,"could not resolve hostname %s, %s\n",hostname, err);
		return -1;
	}

	int sfd = create_socket(&addr,0,NULL,0);
	if(sfd == -1){
		fprintf(stderr, "could not create socket : sfd = -1\n");
		return -1;
	}
	int fd;
	if(file==NULL)
	{
		fd=1;
		printf("%i\n",fd);
	}
	else{
		fd=open(file,O_WRONLY|O_CREAT);
	}
}

int sender_selective_repeat(int sockfd, int fd)
{
	struct pollfd pfds[2];
	int nbFd;
	int loop=1;
	int comp=fd;
	printf("%i\n",comp);
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
	return 0;
}
