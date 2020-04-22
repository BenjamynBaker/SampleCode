CC = gcc
CFLAGS = -g
CPPFLAGS = -std=gnu90 -Wall -pedantic
UNAME = $(shell uname)
CROSS_COMPILE=arm-linux-gnueabi-
ARCH= $(shell uname -m)

#linux files
ifeq ("$(shell uname)","Linux") 
all: sample-linux sample-raspi

sample-linux: sample-linux.o linux-lib.a 
	$(CC) -o sample-linux sample-linux.o -L. -llinux-lib

sample-linux.o: sample-linux.c lab1.h
	$(CC) -O -c -I. $(CFLAGS) ${CPPFLAGS} sample-linux.c

linux-lib.o: linux-lib.c
	 $(CC) -O -c -I. $(CFLAGS) $(CPPFLAGS) linux-lib.c

linux-lib.a: linux-lib.o
	 ar -rc liblinux-lib.a linux-lib.o

app: app.o
	${CC} app.o -L/student/cmpt332/pthreads/lib/$(UNAME)$(ARCH)/ -lpthreads -o app

app.o: app.c
	${CC} -O -c -I"/student/cmpt332/pthreads" $(CFLAGS) ${CPFLAGS} app.c 

sample-raspi: sample-raspi.o raspi-lib.a
	${CROSS_COMPILE}$(CC) -o sample-raspi sample-raspi.o -L. -lraspi-lib

sample-raspi.o: sample-linux.c
	${CROSS_COMPILE}$(CC) -c -I. $(CFLAGS) ${CPPFLAGS} sample-linux.c -o sample-raspi.o

raspi-lib.o: linux-lib.c
	${CROSS_COMPILE}$(CC)  -c -I. $(CFLAGS) $(CPPFLAGS) linux-lib.c -o raspi-lib.o

raspi-lib.a: raspi-lib.o
	ar -rc libraspi-lib.a raspi-lib.o
endif

#window files
ifeq ("$(shell uname)","MINGW64_NT-10.0-17763")

wthread: wthread.o thread.o
	$(CC) -o wthread $(CFLAGS) ${CPPFLAGS} wthread.o thread.o

wthread.o: wthread.c
	${CC} -O -c ${CFLAGS} ${CPPFLAGS} wthread.c

thread.o: thread.c
	${CC} -O -c ${CFLAGS} ${CPPFLAGS} thread.c

endif

.PHONY: clean
clean:
	rm  *.a *.o *.exe sample-linux sample-raspi  
