#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>

#include "read_write_loop.h"

void read_write_loop(int sfd) {
    
    
    fd_set sfds;
    char buff[1024];
    FD_ZERO(&sfds);
    
    while(1)
    {
        FD_SET(STDIN_FILENO,&sfds);
        FD_SET(sfd,&sfds);
        
        select(sfd+1,&sfds,NULL,NULL,NULL);
        
        if(FD_ISSET(STDIN_FILENO,&sfds))
        {
            ssize_t r=read(STDIN_FILENO,buff,1024);
            
            if(r == EOF){
                break;
            }
            
            size_t len = (size_t)r;
            int w = (int) write(sfd,buff,len);
            if(w == -1){
                fprintf(stderr, "error with write (read_write_loop)");
            }
            
        }else if(FD_ISSET(sfd,&sfds))
        {
            
            ssize_t r=read(sfd,buff,1024);
            
            if(r == EOF){
                break;
            }
            size_t len = (size_t)r;
            int w = (int) write(STDOUT_FILENO,buff,len);
            
            if(w == -1){
                fprintf(stderr, "error with write (read_write_loop)");
            }
        }
    }
}
