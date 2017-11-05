#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include "wait_for_client.h"
/* Block the caller until a message is received on sfd,
 * and connect the socket to the source addresse of the received message
 * @sfd: a file descriptor to a bound socket but not yet connected
 * @return: 0 in case of success, -1 otherwise
 * @POST: This call is idempotent, it does not 'consume' the data of the message,
 * and could be repeated several times blocking only at the first call.
 */
 int wait_for_client(int sfd)
{
    char buffer[524];
    struct sockaddr_in6 src;
    socklen_t len = (socklen_t) sizeof(struct sockaddr_in6);
    memset(&src,0,sizeof(src));
    fprintf(stderr, " wait_for_client => recvfrom : ?\n");
    int recv = recvfrom(sfd,buffer,524 - 1,MSG_PEEK,(struct sockaddr *) &src,&len);
    if(recv == -1)
    {
      fprintf(stderr, " wait_for_client => ERROR : recvfrom == -1\n");
      return -1;
    }
    fprintf(stderr, " wait_for_client => recvfrom : OK\n");
    fprintf(stderr, " wait_for_client => connect : ?\n");
    recv = connect(sfd,(struct sockaddr *)&src,sizeof(struct sockaddr_in6));
    if(recv == -1)
    {
      fprintf(stderr, " wait_for_client => ERROR : connect == -1\n");
      return -1;
    }
    fprintf(stderr, " wait_for_client => connect : OK\n");
    return 0;
}
