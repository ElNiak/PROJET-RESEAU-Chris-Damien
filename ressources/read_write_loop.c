#include <fcntl.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <unistd.h>
#include "read_write_loop.h"
/* Loop reading a socket and printing to stdout,
 * while reading stdin and writing to the socket
 * @sfd: The socket file descriptor. It is both bound and connected.
 * @return: as soon as stdin signals EOF
 */
void read_write_loop(int sfd)
{
  int nbFd;
  struct pollfd pfds[2];
  char buffer[1024];
  int loop = 1;
  ssize_t a;
  int b;
  while(loop)
  {
    pfds[0].fd = 0;
    pfds[0].events = POLLIN;

    pfds[1].fd = sfd;
    pfds[1].events = POLLIN;

    nbFd = poll(pfds,2,-1); //timeout = -1 => Pour illimite
    if(nbFd == -1)
    {
      fprintf(stderr, "error poll()");
    }
    if (pfds[0].revents & POLLIN)
    {

          a = read(0,buffer,1024);
          if (a == EOF)
          {
              loop = 0;
          }
          b = write(sfd,buffer,a);
          if(b == -1)
          {
              fprintf(stderr, "error write()");
          }
       }
       if (pfds[1].revents & POLLIN) {

          a = read(sfd, buffer, 1024);
          if (a == EOF)
          {
              loop = 0;
          }
          b =  write(1, buffer, a);
          if(b == -1)
          {
              fprintf(stderr, "error write()");
          }
       }
  }
}
