AC	= wla-gb
AFLAGS	= -o
LD	= wlalink
LDFLAGS	= -v

SFILES	= bootrom.s
OFILES	= bootrom.o
BIGROMFILE	= bigrom.bin
CUTROMFILE	= bootrom.bin

all: $(CUTROMFILE)

$(CUTROMFILE): $(BIGROMFILE)
	dd if=$(BIGROMFILE) of=$(CUTROMFILE) bs=2304 count=1

$(BIGROMFILE): $(OFILES) Makefile
	$(LD) $(LDFLAGS) Linkfile $(BIGROMFILE)

bootrom.o: bootrom.s
	$(AC) $(AFLAGS) bootrom.s

$(OFILES): $(IFILES)

clean:
	rm -f $(OFILES) *~ $(BIGROMFILE) $(CUTROMFILE)
