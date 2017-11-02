#include "real_address.h"
#include "create_socket.h"
#include "wait_for_client.h"
#include "read_write_loop.h"
#include "packet_interface.h"
#include <errno.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <getopt.h>
#include <poll.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <zlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

char *file=NULL;
char *hostname=NULL;
char *port=NULL;

void get_args(int argc, char **argv){

	int opt;
	if (argc < 2) {
		fprintf(stderr, "`%s' arguments missing\n", argv[0]);
		fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
		exit(1);
	}
	else {
		/* specify which parameters we expect */
		static struct option options[] =
		{
			{"filename", required_argument, 0, 'f'},
			{"help", no_argument, NULL, 'h'},
			{NULL, 0, NULL, 0}
		};

		while ((opt = getopt_long(argc, argv, "f:h:", options, NULL)) != -1) {
			switch (opt) {
				case 'f':
				file = optarg;
				break;
				case 'h':
				printf("\n");
				printf("Usage: %s hostname port\n", argv[0]);
				printf("Usage: %s [-f X] hostname port\n", argv[0]);
				printf("Usage: %s [--filename X] hostname port\n", argv[0]);
				printf("\n");
				exit(1);
				default:
				fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
				exit(1);
			}
		}

		hostname = argv[argc-2];
		port = argv[argc-1];
	}

}


int sender_SR(int sockfd, int fd)
{
	pkt_t *snd_pkt[MAX_WINDOW_SIZE];
	int nbAck = -1;
	uint16_t last_ack = 0;
	for(int i = 0; i < 31; i++)
	{
		snd_pkt[i] = NULL;
	}

	struct timeval * time_buffer[MAX_WINDOW_SIZE];
	for(int i = 0; i < 31; i++)
	{
		time_buffer[i] = NULL;
	}

	uint8_t window = 1; // 0->31
	uint8_t max_wind = 1;
	int isUpdate = 0;
	uint8_t seqnum = 0;
	int nbReadPack = -1;
	char payload[MAX_PAYLOAD_SIZE];
	char packet[MAX_PAYLOAD_SIZE+12];
	char acknowledgement[12];
	//int mfd = sockfd+1;
	struct pollfd pfds[2];
	int loop = 1;
	ssize_t reads;
	int nbFd;
	int loopSend = 0;
	fprintf(stderr, "sender => sender_SR() : loop : ?\n");
	while(loop)
	{
		pfds[0].fd = sockfd;
		pfds[0].events =  POLLOUT;

		pfds[1].fd = sockfd;
		pfds[1].events = POLLOUT;
		fprintf(stderr, "sender => sender_SR() : poll : ?\n");
		nbFd = poll(pfds,2,-1); //timeout = -1 => Pour illimite
		if(nbFd == -1)
		{
			fprintf(stderr, "sender => error poll() - 1");
			return -1;
		}
		fprintf(stderr, "sender => sender_SR() : poll : OK\n");
		if(pfds[0].revents & POLLOUT)
		{
			fprintf(stderr, "sender => sender_SR() : recv : ?\n");
			nbAck = recv(sockfd,acknowledgement,12,0);
			if(nbAck == -1)
			{
				fprintf(stderr, "sender => error rcv()");
				return -1;
			}
			fprintf(stderr, "sender => sender_SR() : recv : OK\n");
			pkt_t * ack = pkt_new();
			fprintf(stderr, "sender => sender_SR() : pkt_decode 1 : ?\n");
			pkt_status_code status = pkt_decode(acknowledgement,12,ack);
			if(status != PKT_OK)
			{
				fprintf(stderr, "sender => error pkt_decode()");
				return -1;
			}
			fprintf(stderr, "sender => sender_SR() : pkt_decode : OK\n");
			uint8_t res = pkt_get_window(ack);
			if(res != max_wind && !isUpdate)
			{
				uint8_t new_place = pkt_get_window(ack) - max_wind;
				window = window + new_place;
				max_wind = pkt_get_window(ack);
				isUpdate = 1;
			}

			last_ack = pkt_get_window(ack);
			for(int i = 0; i < max_wind;i++)
			{
				if(snd_pkt[i] != NULL)
				{
					uint8_t seqnum_pkt = pkt_get_seqnum(snd_pkt[i]);

					if(last_ack > seqnum_pkt && (last_ack - seqnum_pkt) <= max_wind)
					{
						pkt_del(snd_pkt[i]);
						snd_pkt[i] = NULL;
						time_buffer[i] = NULL;
						window++;
					}
					else if(last_ack < seqnum_pkt && (last_ack + 255 - seqnum_pkt) <= max_wind)
					{
						pkt_del(snd_pkt[i]);
						snd_pkt[i] = NULL;
						time_buffer[i] = NULL;
						window++;
					}
				}
			}
		}
		if(pfds[1].revents & POLLOUT) //envoie de packet
		{
			if(window > 0 && !loopSend)
			{
				memset(payload,0,512);
				nbReadPack = read(fd,payload,512);
				if(nbReadPack == 0)
				{
					loopSend = 1;
				}
				else if(nbReadPack == 1 && payload[0] == '\n')
				{
					loopSend = 1;
				}
				pkt_t* new = pkt_new();
				pkt_set_length(new,nbReadPack);
				pkt_set_type(new,PTYPE_DATA);
				pkt_set_seqnum(new,seqnum);
				seqnum = (seqnum+1)%256;
				pkt_set_window(new,window);
				pkt_set_timestamp(new,2);
				pkt_set_payload(new,payload,nbReadPack);
				pkt_status_code status;
				size_t len = nbReadPack+12;
				memset(packet,0,524);
				status = pkt_encode(new,packet,&len);
				if(status != PKT_OK)
				{
					fprintf(stderr, "sender => error pkt_encode() - 1");
					return -1;
				}
				uint8_t pos_buffer = 0;
				while(pos_buffer < max_wind && snd_pkt[pos_buffer] != NULL)
				{
					pos_buffer++;
				}
				snd_pkt[pos_buffer] = new;
				window--;

				reads = send(sockfd,packet,len,0);
				struct timeval * ntime = malloc(sizeof(struct timeval));
				gettimeofday(ntime,NULL);
				time_buffer[pos_buffer] = ntime;

				if(reads == -1)
				{
					fprintf(stderr, "sender => error send() - 1");
					return -1;
				}
			}
		}
		if(pfds[1].revents & POLLOUT) // packet perdu
		{
			for(int i = 0; i < max_wind ; i++)
			{
				if(time_buffer[i] != NULL)
				{
					struct timeval * now = malloc(sizeof(struct timeval));
					gettimeofday(now,NULL);
					if((now->tv_sec - time_buffer[i]->tv_sec)*1000 - (now->tv_usec - time_buffer[i]->tv_usec)/1000 > 1000) //1000 = timeout
					{
						size_t len = pkt_get_length(snd_pkt[i])+12;
						memset(packet,0,524);
						pkt_status_code status = pkt_encode(snd_pkt[i],packet,&len);
						if(status != PKT_OK)
						{
							fprintf(stderr, "sender => error pkt_encode() - 2");
							return -1;
						}

						reads = send(sockfd,packet,len,0);
						gettimeofday(time_buffer[i],NULL);
						if(reads == -1)
						{
							fprintf(stderr, "sender => error send() - 2");
							return -1;
						}
					}
					free(now);
				}
			}
		}

		if(window == max_wind && loopSend)
		{
			loop = 0;
		}
	}
	fprintf(stderr, "sender => sender_SR() : loop : OK\n");
	int disconnected = 0;
	while (!disconnected)
	{
		pfds[0].fd = sockfd;
		pfds[0].events = POLLIN | POLLPRI | POLLOUT;

		pfds[1].fd = sockfd;
		pfds[1].events = POLLIN | POLLPRI | POLLOUT;

		nbFd = poll(pfds,2,-1); //timeout = -1 => Pour illimite
		if(nbFd == -1)
		{
			fprintf(stderr, "sender => error poll() - 2");
			return -1;
		}

		if(pfds[0].revents & POLLOUT)
		{
			memset(acknowledgement,0,12);
			nbAck = recv(sockfd,acknowledgement,12,0);
			pkt_t * ack2 = pkt_new();
			pkt_status_code status = pkt_decode(acknowledgement,12,ack2);
			if(status != PKT_OK)
			{
				fprintf(stderr, "sender => error decode - 4\n");
				return -1;
			}
			if(nbAck == 12)
			{
				disconnected = 1;
			}
		}

		if(pfds[1].revents & POLLOUT && ! disconnected)
		{
			char bufack[12];
			if(time_buffer[0] == NULL)
			{
				pkt_t* disco = pkt_new();
				pkt_set_length(disco,0);
				pkt_set_type(disco,PTYPE_DATA);
				pkt_set_seqnum(disco,seqnum);
				pkt_set_window(disco,window);
				pkt_set_timestamp(disco,2);
				memset(bufack,0,12);
				size_t len = 12;
				pkt_status_code code = pkt_encode(disco,bufack,&len);
				if(code != PKT_OK)
				{
					fprintf(stderr, "sender => error encode() - 3\n");
					return -1;
				}

				snd_pkt[0] = disco;
				reads = send(sockfd,bufack,len,0);
				struct timeval * newtime = malloc(sizeof(struct timeval));
				gettimeofday(newtime,NULL);
				time_buffer[0] = newtime;
				if(reads == -1)
				{
					fprintf(stderr, "sender => error send() -3\n");
					return -1;
				}
			}
			else
			{
				struct timeval *now = malloc(sizeof(struct timeval));
				gettimeofday(now,NULL);
				if((now->tv_sec - time_buffer[0]->tv_sec)*1000 - (now->tv_usec - time_buffer[0]->tv_usec)/1000 > 1000)
				{
					size_t len = 12;
					memset(bufack,0,12);
					pkt_status_code code = pkt_encode(snd_pkt[0],bufack,&len);
					if(code != PKT_OK)
					{
						fprintf(stderr, "sender => error encode() - 4\n");
						return -1;
					}
					reads = send(sockfd,bufack,len,0);
					if(reads == -1)
					{
						disconnected = 1;
					}
					gettimeofday(time_buffer[0],NULL);
				}
				free(now);
			}
		}

	}
	close(fd);
	close(sockfd);
	return 0;
}



char *get_ip_str(struct sockaddr_in6 *sa, char *s, size_t maxlen)
{
	inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),s, maxlen);
	return s;
}

int main(int argc, char **argv){
	fprintf(stderr, "sender => main() : get_args : ?\n");
	get_args(argc,argv);
	fprintf(stderr, "sender => main() : get_args : OK\n");
	struct sockaddr_in6 addr;
	fprintf(stderr, "sender => main() : real_address : ?\n");
	const char *err=real_address(hostname,&addr);
	fprintf(stderr, "sender => main() : real_address : OK\n");
	if(err)
	{
		fprintf(stderr,"sender => could not resolve hostname %s, %s\n",hostname, err);
		return -1;
	}
	int port_int=atoi(port);
	fprintf(stderr, "== sender => port  : %d\n",port_int);
	char res [50];
	get_ip_str(&addr,res,50);
	fprintf(stderr, "== sender => ipv6  : %s\n",res);
	fprintf(stderr, "sender => main() : create_socketv2 : ?\n");
	int sfd = create_socket(NULL,-1,&addr,port_int);
	fprintf(stderr, "sender => main() : create_socketv2 : OK\n");
	if(sfd == -1){
		fprintf(stderr, "sender => could not create socket : sfd = -1\n");
		return -1;
	}
	int fd;
	FILE *f;
	if(file==NULL)
	{
		fd=STDIN_FILENO;
	}
	else{
		f=fopen(file,"r");
		fd=open(file,O_RDONLY);
	}
	fprintf(stderr, "sender => main() : sender_SR : ?\n");
	int err_sr=sender_SR(sfd,fd);
	if(err_sr!=0){
		fprintf(stderr,"error while executing sender_SR");
	}
	fprintf(stderr, "sender => main() : sender_SR : OK\n");
	return 0;
}
