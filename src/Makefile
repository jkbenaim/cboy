target = cboy
csources = $(wildcard *.c)
cppsources = $(wildcard *.cpp)
objects = $(csources:.c=.o) $(cppsources:.cpp=.o) bootrom.o gb_apu/libgb_apu.a
CFLAGS= -std=c99 -Os -ggdb -Wall -Werror `sdl2-config --cflags`
CXXFLAGS= -Os -ggdb -Wall `sdl2-config --cflags`

.PHONY: all
all: $(target)

.INTERMEDIATE: bootrom.c

$(target): Makefile $(objects)
	$(CXX) $(CFLAGS) $(objects) `sdl2-config --libs` -o $(target)

.PHONY: clean
clean:
	rm -Rf $(target) *.o
	$(MAKE) -C gb_apu clean
	$(MAKE) -C tools clean

apu_sources = $(wildcard gb_apu/*.cpp)
gb_apu/libgb_apu.a: Makefile
	$(MAKE) -C gb_apu

tools/bin2c: Makefile
	$(MAKE) -C tools

bootrom.c: ../bootrom/bootrom.bin tools/bin2c
	tools/bin2c bootrom_bin < $< > $@
