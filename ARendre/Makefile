GCC = gcc
CFLAGS = -std=gnu99 -g 
OPT= -Werror -W -Wall
OBJ = src/real_address.o src/create_socket.o src/packet_implement.o src/wait_for_client.o
HFILES= src/packet_interface.h src/real_address.h src/wait_for_client.h src/create_socket.h

all : receiver sender clear

tests : $(OBJ) tests/test.o
	$(GCC) -o $@ $^ $(CFLAGS)  

receiver :  $(OBJ)  src/receiver.o 
	$(GCC) -o $@ $^ $(CFLAGS) -lz 

sender :  $(OBJ)  src/sender.o
	$(GCC) -o $@ $^ $(CFLAGS) -lz 

%.o: src/%.c $(HFILES)
	$(GCC) -c $@ $< $(CFLAGS) 

%.o: tests/%.c $(HFILES)
	$(GCC) -c $@ $< $(CFLAGS) 


clear :
	rm -rf src/*.o tests/*.o

mrproper :
	rm -rf $(EXEC)
