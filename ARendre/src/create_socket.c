#include <netinet/in.h> /* * sockaddr_in6 */
#include <sys/types.h> /* sockaddr_in6 */
#include <stdlib.h>
int create_socket(struct sockaddr_in6 *source_addr,  int src_port,struct sockaddr_in6 *dest_addr,int dst_port){
    int sockfd;
    sockfd = socket(AF_INET6,SOCK_DGRAM,0);
    if(sockfd <0) return -1;

    if(source_addr != NULL)
    {
      if(src_port > 0)
      {
        source_addr->sin6_port  = htons(src_port);
      }
      if(bind(sockfd,(struct sockaddr *) source_addr, 		sizeof(struct sockaddr_in6)) < 0)
      {
        return -1;
      }
    }
    if(dest_addr != NULL)
    {
      if(dst_port > 0)
      {
        dest_addr->sin6_port = htons(dst_port);
      }
      if(connect(sockfd, (struct sockaddr *) dest_addr, sizeof(struct sockaddr_in6)) < 0)
      {
        return -1;
      }
    }
    return sockfd;

  }