#include "real_address.h"
#include "create_socket.h"
#include "wait_for_client.h"
#include "read_write_loop.h"
#include <stdlib.h>
#include <netinet/in.h>

char *file=NULL;
char *hostname=NULL;
int port=-1;


int acknowledgement(uint8_t window, int sockfd, uint8_t seqnum)
{

  pkt_t *ackgmt_pkt = pkt_new();//On cree puis initialise le packet
  pkt_set_length(ackgmt_pkt,0);
  pkt_set_type(ackgmt_pkt,PTYPE_ACK);
  pkt_set_window(ackgmt_pkt,window);
  pkt_set_seqnum(ackgmt_pkt,seq_num);

  char * ackgmt_mess = calloc(12,sizeof(char));
  size_t length = 12;
  pkt_status_code code = pkt_encode(ackgmt_pkt,ackgmt_mess,&length);

  if(code != PKT_OK)
  {
    return -1; //Erreur dans le packet
  }
  else
  {
    int err = send(sockfd, ackgmt_mess,sizeof(ackgmt_mess),0);
    if(err <0)
    {
      return -1; //erreur dans l'envoie de l'accusé de reception
    }
  }
  pkt_del(ackgmt_pkt);//suppression du packet
  return 0;
}



int receiver_SR(int sockfd, int fd)
{
  uint8_t window = MAX_WINDOW_SIZE; //31
  pkt_t *rcv_pkt[window]; //buffer avec les futur frame à traiter

  for(int i = 0; i < 31; i++)
  {
    rcv_pkt[i] = NULL;
  }

  uint8_t seqnum = 0;
  ssize_t read;
  char *buffer;
  struct pollfd pfds[2];
  int loop = 1;
  int nbFd;

  while(loop)
  {
    pfds[0].fd = sockfd;
    pfds[0].events = POLLIN | POLLPRI;

    pfds[1].fd = sockfd;
    pfds[1].events = POLLIN | POLLPRI;

    nbFd = poll(pfds,2,-1); //timeout = -1 => Pour illimite
    if(nbFd == -1)
    {
      fprintf(stderr, "error poll()");
      return -1;
    }



    if ((pfds[0].revents & POLLIN) && (pfds[1].revents & POLLIN))
    { // check for events on sockfd read :
      buffer = calloc(524,sizeof(char));
      read = recv(sdf,buffer,MAX_PAYLOAD_SIZE+12,0); //nb de byte lu
      if(read > 0)
      {
        pkt_t new = pkt_new();
        pkt_status_code decode = pkt_decode(buffer, read, new);

        if(read == 12) //les 12 bits en plus de payload
        {
          if(pkt_get_seqnum(new) == seqnum)
          {
            acknowledgement(sdf,0,(seqnum+1)%256);
            loop = 0;
          }
          else
          {
            acknowledgement(sfd,0,seqnum);
          }
        }
        else if(decode != PKT_OK) //Erreur dans le packet
        {
          if(decode == E_CRC)
          {
            acknowledgement(sfd,window,seqnum);
          }
        }
        else
        {
          if(pkt_get_seqnum(new) == seqnum)
          { //write pck payload
            write(fd,pkt_get_payload(new),pkt_get_length(new));
            seqnum = (seqnum+1)%256;

            int cont = 1;
            while(cont)
            {
              cont = 0;
              for(int i = 0; i < window && cont == 0 ; i++)
              {
                if(rcv_pkt[i] != NULL)
                {
                  if(pkt_get_seqnum(rcv_pkt[i]) == seqnum)
                  {
                    write(fd,pkt_get_payload(rcv_pkt[i]),pkt_get_length(rcv_pkt[i]));
                    seqnum = (seqnum+1)%256;
                    rcv_pkt[i] = NULL;
                    cont = 1;
                  }
                }
              }
            }
            acknowledgement(sfd,window,seqnum);
          }
          else if(((pkt_get_seqnum(new) > seqnum) && ((pkt_get_seqnum(new) - seqnum) <= window)) || ((pkt_get_seqnum(new) < seqnum) && ((pkt_get_seqnum(new) + 255 - seqnum) <= window))
          { // packet dans le desordre
            int isIN = 0;
            for(int i = 0; i < window && !isIn ; i++)
            {
              if(rcv_pkt[i] != NULL)
              {
                isIn = (pkt_get_seqnum(rcv_pkt[i]) == pkt_get_seqnum(new));
              }
            }
            if(!isIn)
            {
              uint8_t pos = 0;
              while(pos < window && rcv_pkt[pos] != NULL)
              {
                pos++;
              }
              rcv_pkt[pos] = new;
            }
            acknowledgement(sdf,window,seq_num);
          }
          else
          {
            acknowledgement(sdf, window,seq_num);
          }
        }
      }
    }
  }
  close(fd);
  close(sfd);
  return 0;
}


int main(int argc, char *argv[])
{
  get_args(argc,argv);
  struct sockaddr_in6 addr;
	int err=real_address(hostname,&addr);
	if(err){
		fprintf(stderr,"error while resolving hostname to a sockaddr_in6");
	}

  int sfd = create_socket(&addr,NULL);
  if(sfd == -1) return -1;

  if(wait_for_client(sfd) < 0)
  {
    close(sfd);
    return -1;
  }

  int fd;
  if(file == NULL) //filename a recuperer dans les argc
  {
    fd = 1;
  }
  else
  {
    fd = open(file,O_WRONLY|O_CREAT);
  }

  int err = receiver_SR(sfd,fd);
  if(err == -1) return -1;

  return 0;
}
