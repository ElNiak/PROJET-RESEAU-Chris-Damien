GCC = gcc
CFLAGS = -Wall -W -Werror -std=gnu99 -g 
OBJ = src/real_address.o src/create_socket.o src/packet_implement.o src/wait_for_client.o
hFILES= src/packet_interface.h src/real_address.h src/wait_for_client.h src/create_socket.h

all : receiver sender tests clear

tests : tests/test.o
	$(CC) -o $@ $^ $(CFLAGS)  

receiver :  $(OBJ)  src/receiver.o 
	$(GCC) -o $@ $^ $(CFLAGS) -lz 

sender :  $(OBJ)  src/sender.o
	$(GCC) -o $@ $^ $(CFLAGS) -lz 

%.o: src/%.c $(HFILES)
	$(GCC) -c $@ $< $(CFLAGS) 

%.o: tests/%.c 
	$(GCC) -c $@ $< $(CFLAGS) 


clear :
	rm -rf src/*.o tests/*.o

mrproper :
	rm -rf $(EXEC)
