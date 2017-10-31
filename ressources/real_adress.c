#include <string.h>
#include <netdb.h>
#include "real_adrress.h"
#include <stdlib.h>
#include <stdio.h>

/* Resolve the resource name to an usable IPv6 address
 * @address: The name to resolve
 * @rval: Where the resulting IPv6 address descriptor should be stored
 * @return: NULL if it succeeded, or a pointer towards
 *          a string describing the error if any.
 *          (const char* means the caller cannot modify or free the return value,
 *           so do not use malloc!)
 */
const char * real_address(const char *address, struct sockaddr_in6 *rval)
{

struct addrinfo hints, *res;
int status;

memset(&hints,0,sizeof(hints));
hints.ai_family = AF_INET6;
hints.ai_socktype = SOCK_STREAM;

if((status = getaddrinfo(address,NULL,&hints,&res)) != 0)
{
    fprintf(stderr,"real_address => getaddrinfo != 0 : %s:\n",gai_strerror(status));
    return "getaddrinfo error";
}

    struct sockaddr_in6 * addr = (struct sockaddr_in6 *) res->ai_addr;
    memcpy(rval, addr, res->ai_addrlen); // copy the resulting IPv6 address to rval

    freeaddrinfo(res);
    return NULL;
}
