#本工程生成三个目标文件 ： netlayer ip_interface himac_test
all : netlayer ip_interface himac_test
.PHONY : all
CC = gcc
OBJECTS1 = nl_main.o mr_common.o nl_timer.o nl_rcv.o nl_common.o nl_package.o nl_send.o
objects2 = ip_interface.o  mr_common.o
objects3 = himac_test.o  mr_common.o

#生成第一个目标netlayer
netlayer:$(OBJECTS1)
	$(CC) -o netlayer $(OBJECTS1) -lpthread

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

#生成第二个目标ip_interface
ip_interface:$(objects2)
	$(CC) -g -o ip_interface $(objects2) -lpthread
ip_interface.o:ip_interface.c
	$(CC) -c -g ip_interface.c -o ip_interface.o

#生成第三个目标himac_test
himac_test:$(objects3)
	$(CC) -g -o himac_test $(objects3)
himac_test.o:himac_test.c
	$(CC) -c -g himac_test.c -o himac_test.o

clean :
	rm *.o netlayer ip_interface himac_test


