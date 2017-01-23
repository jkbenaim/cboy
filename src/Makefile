target = cboy
sources = $(wildcard *.c)
CC=gcc
CFLAGS= -std=c99 -O3 -ggdb -Wall -Werror `sdl-config --cflags` -DUSE_SDL -U_FORTIFY_SOURCE

.PHONY: all
all: $(target)


ifneq ($(MAKECMDGOALS),clean)
-include $(sources:.c=.d)
endif

$(target): Makefile $(sources:.c=.o)
	$(CC) $(CFLAGS) $(sources:.c=.o) `sdl-config --libs` -o $(target)

clean:
	rm -f $(target) *.o *.d

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
