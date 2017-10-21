#include <netinet/in.h> /* * sockaddr_in6 */
#include <sys/types.h> /* sockaddr_in6 */

const char * real_address(const char *address, struct sockaddr_in6 *rval)7
{

struct addrinfo hints, res *, *p;
int status;

memset(&hints,0,sizeof(hints));
hints.ai_family = AF_INET6;
hints.ai_socktype = SOCK_STREAM;

if((status = getaddrinfo(address,NULL,&hints,&res)) != 0)
{
    fprintf(stderr,"getaddrinfo: %s:\n",gai_strerror(status));
    return "getaddrinfo error";
}
   // p = res;
   // while(p != NULL)
   // {
        rval = (struct sockaddr_in6 *)p->ai_addr;
        addr = &(ipv6->sin6_addr);
       // p = p->ai_next;
   // }
    freeaddrinfo(res);
    return NULL;
}
