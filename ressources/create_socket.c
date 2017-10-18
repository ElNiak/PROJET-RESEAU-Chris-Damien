#include <string.h>
#include <netdb.h>
#include <stdio.h>

#include "create_socket.h"

int create_socket(struct sockaddr_in6 *source_addr,
                 int src_port,
                 struct sockaddr_in6 *dest_addr,
                 int dst_port){
   if(source_addr==NULL||dest_addr==NULL||src_port>0||dst_port>0){
       fprintf(stderr,"error illegal argument");
       return -1;
   }
   int sfd=socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP);
   source_addr->sin6_port=htons(src_port);
   int err=bind(sfd,(struct sockaddr *)source_addr,sizeof(struct sockaddr_in6));
    if(err==-1){
        fprintf(stderr, "cannot bind (create_socket)");
		return -1;
    }
   dest_addr->sin6_port=htons(dst_port);
   err=connect(sfd,(struct sockaddr *)dest_addr,sizeof(struct sockaddr_in6));
     if(err==-1){
        fprintf(stderr, "cannot connect (create_socket)");
		return -1;
    }
   return sfd;    
}
