#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netdb.h>

#include "wait_for_client.h"

int wait_for_client(int sfd){
    char buf[1024];
    struct sockaddr_storage address;
    socklen_t address_len;
    ssize_t nread;
    address_len = sizeof(struct sockaddr_storage);
    nread = recvfrom(sfd, buf, 1024, 0, (struct sockaddr *)&address, &address_len);
    if (nread == -1)  return -1;
    return 0;
}
