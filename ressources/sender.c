#include <stdlib.h>
#include <stdio.h>

#include "sender.h"
#include "real_address.h"
#include "create_socket.h"

int port=-1;
char *hostname;
char *file=NULL;

int main(int argc, char **argv){

	get_args(argc,argv);

	struct sockaddr_in6 addr;
	int err=real_address(hostname,&addr);
	if(err){
		fprintf(stderr,"error while resolving hostname to a sockaddr_in6");
	}
	
}
