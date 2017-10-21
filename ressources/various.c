#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "various.h"

void get_args(int argc, char **argv){

	if(argc<2){
		fprintf(stderr,"not enough arguments\n");
		fprintf(stderr,"use %s --help to get commands",argv[0]);
		exit(42);
	}
	char opt;
	while((opt=getopt(argc,argv,"f:h:",options,NULL)==-1){
		switch(opt){
			case 'f'
			file=optarg;
			break;

			case'h'
			printf("\n");
			printf("Use the '%s hostname port' to send/receive data from the address 'hostname via the UDP 'port' \n",argv[0]);
			printf("Use the '%s [-f X] hostname port' to send the content of a file or to save 7
			the data received in the file\n",argv[0]);
			exit(42);

			default
			fprintf(stderr,"invalid argument(s), use %s --help to get commands", argv[0]);
			exit(42);
		}
	}
	hostname = argv[argc-2];
	port = atoi(argv[argc-1]);
}
