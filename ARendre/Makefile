GCC = gcc
CFLAGS = -Wall -W -Werror -std=gnu99 -g 
OBJ = src/real_address.o src/create_socket.o src/packet_implement.o src/wait_for_client.o
hFILES= src/packet_interface.h src/real_address.h src/wait_for_client.h src/create_socket.h
HOME=/home/raquet/Documents/Projet_reseaux/PROJET-RESEAU-Chris-Damien/ARendre

all : receiver sender tests

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
	rm -rf *.o

mrproper :
	rm -rf $(EXEC)
