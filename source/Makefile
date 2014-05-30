
CC=gcc
CFLAGS= -O3 -ggdb  -Wall -Werror `sdl-config --cflags` -DUSE_SDL -U_FORTIFY_SOURCE

all: cboy

cboy: Makefile cart.o cart_chardev.o cartdesc.o cpu.o input.o main.o mbc_cam.o mbc_huc3.o mbc_mbc1.o mbc_mbc2.o mbc_mbc3.o mbc_mbc5.o mbc_mbc7.o mbc_boot.o mbc_c_boot.o mbc_c_none.o mbc_c_mbc1.o mbc_c_mbc2.o mbc_c_mbc3.o mbc_c_cam.o mbc.o mbc_none.o memory.o pqueue.o serial.o video.o
	$(CC) $(CFLAGS) cart.o cart_chardev.o cartdesc.o cpu.o input.o main.o mbc_cam.o mbc_huc3.o mbc_mbc1.o mbc_mbc2.o mbc_mbc3.o mbc_mbc5.o mbc_mbc7.o mbc_boot.o mbc_c_boot.o mbc_c_none.o mbc_c_mbc1.o mbc_c_mbc2.o mbc_c_mbc3.o mbc_c_cam.o mbc.o mbc_none.o memory.o pqueue.o serial.o video.o `sdl-config --libs` -o cboy

clean:
	rm -f cboy gbinfo *.o

distclean: clean
	rm -f *~

run: cboy
	./cboy ../roms/cgb_rom.bin ../roms/pkmnsilv.gbc

debug: cboy
	gdb --args ./cboy ../roms/dmg_rom.bin ../roms/pkmnsilv.gbc

profile: cboy run
	gprof cboy > profile.txt

audio.o: Makefile audio.c audio.h
	$(CC) $(CFLAGS) -c audio.c

cart.o: Makefile cart.c cart.h cart_chardev.h memory.h mbc.h mbc_boot.h cartdesc.h cpu.h
	$(CC) $(CFLAGS) -c cart.c

cart_chardev.o: Makefile cart.c cart.h cart_chardev.c cart_chardev.h memory.h mbc.h mbc_boot.h cartdesc.h cpu.h
	$(CC) $(CFLAGS) -c cart_chardev.c

cartdesc.o: Makefile cartdesc.c cartdesc.h
	$(CC) $(CFLAGS) -c cartdesc.c

cpu.o: Makefile cpu.c cpu.h memory.h assert.h endian.h serial.h pqueue.h
	$(CC) $(CFLAGS) -c cpu.c

input.o: Makefile input.c types.h main.h memory.h input.h cpu.h
	$(CC) $(CFLAGS) -c input.c

main.o: Makefile main.c audio.h types.h cpu.h memory.h video.h cart.h main.h
	$(CC) $(CFLAGS) -c main.c

mbc_cam.o: Makefile mbc_cam.c memory.h cart.h mbc_cam.h
	$(CC) $(CFLAGS) -c mbc_cam.c

mbc_huc3.o: Makefile mbc_huc3.c memory.h cart.h mbc_huc3.h assert.h
	$(CC) $(CFLAGS) -c mbc_huc3.c

mbc_mbc1.o: Makefile mbc_mbc1.c memory.h cart.h mbc_mbc1.h
	$(CC) $(CFLAGS) -c mbc_mbc1.c

mbc_mbc2.o: Makefile mbc_mbc2.c memory.h cart.h mbc_mbc2.h
	$(CC) $(CFLAGS) -c mbc_mbc2.c

mbc_mbc3.o: Makefile mbc_mbc3.c memory.h cart.h mbc_mbc3.h assert.h
	$(CC) $(CFLAGS) -c mbc_mbc3.c

mbc_mbc5.o: Makefile mbc_mbc5.c memory.h cart.h mbc_mbc5.h assert.h
	$(CC) $(CFLAGS) -c mbc_mbc5.c

mbc_mbc7.o: Makefile mbc_mbc7.c memory.h cart.h mbc_mbc7.h assert.h
	$(CC) $(CFLAGS) -c mbc_mbc7.c

mbc_boot.o: Makefile mbc_boot.c memory.h cart.h mbc_boot.h
	$(CC) $(CFLAGS) -c mbc_boot.c

mbc_c_boot.o: Makefile mbc_c_boot.c mbc_c_boot.h memory.h cart.h
	$(CC) $(CFLAGS) -c mbc_c_boot.c

mbc_c_none.o: Makefile mbc_c_none.c mbc_c_none.h memory.h cart.h
	$(CC) $(CFLAGS) -c mbc_c_none.c

mbc_c_mbc1.o: Makefile mbc_c_mbc1.c mbc_c_mbc1.h memory.h cart.h
	$(CC) $(CFLAGS) -c mbc_c_mbc1.c

mbc_c_mbc2.o: Makefile mbc_c_mbc2.c mbc_c_mbc2.h memory.h cart.h
	$(CC) $(CFLAGS) -c mbc_c_mbc2.c

mbc_c_mbc3.o: Makefile mbc_c_mbc3.c mbc_c_mbc3.h memory.h cart.h
	$(CC) $(CFLAGS) -c mbc_c_mbc3.c

mbc_c_cam.o: Makefile mbc_c_cam.c mbc_c_cam.h memory.h cart.h
	$(CC) $(CFLAGS) -c mbc_c_cam.c

mbc.o: Makefile mbc.c mbc.h types.h
	$(CC) $(CFLAGS) -c mbc.c

mbc_none.o: Makefile mbc_none.c memory.h cart.h mbc_none.h
	$(CC) $(CFLAGS) -c mbc_none.c

memory.o: Makefile memory.c types.h memory.h cpu.h video.h input.h cart.h mbc.h assert.h
	$(CC) $(CFLAGS) -c memory.c

pqueue.o: Makefile pqueue.c pqueue.h
	$(CC) $(CFLAGS) -c pqueue.c
	
serial.o: Makefile serial.c types.h memory.h cpu.h serial.h
	$(CC) $(CFLAGS) -c serial.c

video.o: Makefile video.c types.h video.h memory.h cpu.h assert.h
	$(CC) $(CFLAGS) -c video.c
