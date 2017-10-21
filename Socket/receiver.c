#include "real_address.h"
#include "create_socket.h"
#include "wait_for_client.h"
#include "read_write_loop.h"

void receiver(char * hostname,int port,char * filename)
{
    struct sockaddr_in6 *sock;
    const char * res = real_address(hostname,sock);
    if(res != NULL)
    {
      return;
    }
    int fd = create_socket(NULL,0,sock,port);
    if(fd == -1)
    {
      return;
    }
    int err = 0;

    err = wait_for_client(fd);
    if(err == -1)
    {
      return;
    }
    read_write_loop(fd);
}
