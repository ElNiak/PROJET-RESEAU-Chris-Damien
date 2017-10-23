GCC = gcc
CFLAGS = -Wall -W -Werror -std=gnu99 -g -c
OBJ = packet_interface.o real_address.o create_socket.o packet_implement.o wait_for_client.o

all : receiver sender

tests : receiver sender test 

receiver : receiver.o
	$(GCC) -o receiver receiver.o $(OBJ) -lz

sender : sender.o
	$(GCC) -o sender sender.o $(OBJ) -lz

receiver.o : $(OBJ) receiver.c receiver.h
  $(GCC) -c receiver.c $(CFLAGS)

sender.o : $(OBJ) sender.c sender.h
  $(GCC) -c sender.c $(CFLAGS)

packet_interface.o : packet_implement.c packet_interface.h
  $(GCC) -c packet_interface.c $(CFLAGS)

wait_for_client.o : wait_for_client.c wait_for_client.h
  $(GCC) -c wait_for_client.c $(CFLAGS)

real_address.o : real_address.c real_address.h
  $(GCC) -c real_address.c $(CFLAGS)

create_socket.o : create_socket.c create_socket.h
  $(GCC) -c create_socket.c $(CFLAGS)

test : test.o sender.o receiver.o $(OBJ)
  $(GCC) -o test  test.o receiver.o sender.o $(OBJ) -lz -lcunit

test.o : sender.o receiver.o $(OBJ)
  $(GCC) -c test.c $(CFLAGS)

clear :
  rm -rf *.o

mrproper :
  rm -rf $(EXEC)
