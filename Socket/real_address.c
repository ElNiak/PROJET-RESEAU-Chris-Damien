#include <string.h>
#include <netdb.h>
#include "real_address.h"
#include <stdlib.h>
#include <stdio.h>
const char * real_address(const char *address, struct sockaddr_in6 *rval)
{

struct addrinfo hints, *res;
int status;

memset(&hints,0,sizeof(hints));
hints.ai_family = AF_INET6;
hints.ai_socktype = SOCK_STREAM;

if((status = getaddrinfo(address,NULL,&hints,&res)) != 0)
{
    fprintf(stderr,"getaddrinfo: %s:\n",gai_strerror(status));
    return "getaddrinfo error";
}
    //rval = (struct sockaddr_in6 *)res->ai_addr;
    struct sockaddr_in6 * addr = (struct sockaddr_in6 *) res->ai_addr;
    memcpy(rval, addr, res->ai_addrlen); // copy the resulting IPv6 address to rval

    freeaddrinfo(res);
    return NULL;
}
