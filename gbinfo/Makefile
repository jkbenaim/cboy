
CC=gcc
CFLAGS= -ggdb  -Wall -Werror

all: gbinfo

clean:
	rm -f gbinfo *.o

gbinfo: Makefile gbinfo.c cartdesc.o
	$(CC) gbinfo.c cartdesc.o -o gbinfo
	
cartdesc.o: Makefile cartdesc.c cartdesc.h
	$(CC) $(CFLAGS) -c cartdesc.c

