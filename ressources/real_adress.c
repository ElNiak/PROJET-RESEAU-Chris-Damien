#include <string.h>
#include <netdb.h>

#include "real_address.h"
const char * real_address(const char *address, struct sockaddr_in6 *rval){
struct addrinfo hints;
struct addrinfo *get;
int succes;

memset(&hints,0, sizeof (struct addrinfo));
hints.ai_family=AF_INET6;
hints.ai_socktype=SOCK_DGRAM;
hints.ai_protocol=0;
hints.ai_flags=AI_PASSIVE;
hints.ai_canonname=NULL;
hints.ai_addr=NULL;
hints.ai_next=NULL;

succes=getaddrinfo(address,NULL,&hints,&get);

if(!succes) return gai_strerror(succes);

struct sockaddr_in6 *adrrIPv6=(struct sockaddr_in6 *) get->ai_addr;
memcpy(rval,adrrIPv6,get->ai_addrlen);
freeaddrinfo(get);
return NULL;  
}

