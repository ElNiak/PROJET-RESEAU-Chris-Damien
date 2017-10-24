GCC = gcc
CFLAGS = -Wall -W -Werror -std=gnu99 -g -c -lz
OBJ = packet_interface.o real_address.o create_socket.o packet_implement.o wait_for_client.o

all : receiver sender

tests : receiver sender test

receiver : receiver.o
	$(GCC) -o receiver receiver.o $(OBJ) 

sender : sender.o
	$(GCC) -o sender sender.o $(OBJ)

receiver.o : $(OBJ) src/receiver.c src/receiver.h
	$(GCC) -c src/receiver.c $(CFLAGS)

sender.o : $(OBJ) src/sender.c src/sender.h
	$(GCC) -c src/sender.c $(CFLAGS)

packet_interface.o : src/packet_implement.c src/packet_interface.h 
	$(GCC) -c src/packet_implement.c $(CFLAGS) 

wait_for_client.o : src/wait_for_client.c src/wait_for_client.h
	$(GCC) -c src/wait_for_client.c $(CFLAGS)

real_address.o : src/real_address.c src/real_address.h
	$(GCC) -c src/real_address.c $(CFLAGS)

create_socket.o : src/create_socket.c src/create_socket.h
	$(GCC) -c src/create_socket.c $(CFLAGS)

test : test.o sender.o receiver.o $(OBJ)
	$(GCC) -o test  test.o receiver.o sender.o $(OBJ) -lcunit

test.o : sender.o receiver.o $(OBJ)
	$(GCC) -c tests/test.c $(CFLAGS)

clear :
	rm -rf *.o

mrproper :
	rm -rf $(EXEC)
