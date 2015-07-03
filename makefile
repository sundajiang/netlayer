CC = gcc
OBJECTS = nl_main.o mr_common.o nl_timer.o nl_rcv.o nl_common.o nl_package.o nl_send.o

netlayer:$(OBJECTS)
	$(CC) -o netlayer $(OBJECTS) -lpthread

nl_main.o: nl_main.c
	$(CC) -c nl_main.c

nl_common.o: nl_common.c
	$(CC) -c nl_common.c

mr_commom.o: mr_common.c
	$(CC) -c mr_common.c

nl_timer.o: nl_timer.c
	$(CC) -c nl_timer.c

nl_rcv.o: nl_rcv.c
	$(CC) -c nl_rcv.c

nl_package.o: nl_package.c
	$(CC) -c nl_package.c

nl_send.o: nl_send.c
	$(CC) -c nl_send.c

clean :
	rm $(OBJECTS)  netlayer


