target = cboy
csources = $(wildcard *.c)
cppsources = $(wildcard *.cpp)
objects = $(csources:.c=.o) $(cppsources:.cpp=.o) bootrom.o gb_apu/libgb_apu.a
CFLAGS= -std=c99 -O3 -ggdb -Wall -Werror `sdl2-config --cflags` -DUSE_SDL
CXXFLAGS= -O3 -ggdb -Wall `sdl2-config --cflags` -DUSE_SDL

.PHONY: all
all: $(target)


ifneq ($(MAKECMDGOALS),clean)
-include $(sources:%.c=deps/%.d)
endif

$(target): Makefile $(objects)
	$(CXX) $(CFLAGS) $(objects) `sdl2-config --libs` -o $(target)

.PHONY: clean
clean:
	rm -Rf $(target) *.o deps
	make -C gb_apu clean

deps/%.d: %.c
	@mkdir -p deps
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

apu_sources = $(wildcard gb_apu/*.cpp)
gb_apu/libgb_apu.a: Makefile
	make -C gb_apu
