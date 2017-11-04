#include <netinet/in.h> /* * sockaddr_in6 */
#include <sys/types.h> /* sockaddr_in6 */
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
/* Creates a socket and initialize it
 * @source_addr: if !NULL, the source address that should be bound to this socket
 * @src_port: if >0, the port on which the socket is listening
 * @dest_addr: if !NULL, the destination address to which the socket should send data
 * @dst_port: if >0, the destination port to which the socket should be connected
 * @return: a file descriptor number representing the socket,
 *         or -1 in case of error (explanation will be printed on stderr)
 */
int create_socket(struct sockaddr_in6 *source_addr,  int src_port,struct sockaddr_in6 *dest_addr,int dst_port){
    int sockfd;
    sockfd = socket(PF_INET6,SOCK_DGRAM,IPPROTO_UDP);
    if(sockfd <0)
    {
      fprintf(stderr, " create_socket => ERROR : sockfd < 0\n");
      return -1;
    }

    if(source_addr != NULL)
    {
      source_addr->sin6_family = AF_INET6;
      if(src_port > 0)
      {
        uint16_t port = htons(src_port);
        memcpy(&source_addr->sin6_port,&port,sizeof(uint16_t));
        //source_addr->sin6_port  = htons(src_port);
      }
      if(bind(sockfd,(struct sockaddr *) source_addr, sizeof(struct sockaddr_in6)) != 0)
      {
        fprintf(stderr, " create_socket => ERROR : source_addr == NULL\n");
        return -1;
      }
    }
    if(dest_addr != NULL)
    {
      dest_addr->sin6_family = AF_INET6;
      if(dst_port > 0)
      {
        uint16_t port = htons(dst_port);
        memcpy(&dest_addr->sin6_port,&port,sizeof(uint16_t));
        //dest_addr->sin6_port = htons(dst_port);
      }
      if(connect(sockfd, (struct sockaddr *) dest_addr, sizeof(struct sockaddr_in6)) != 0)
      {
        fprintf(stderr, " create_socket => ERROR : dst_addr == NULL\n");
        return -1;
      }
    }
    return sockfd;
  }
