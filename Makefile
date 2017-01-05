# flags #
CC		= gcc
CFLAGS	= -g -Wall -std=c99
CCOM	= -fPIC -c
CLIB	= -shared

# elf #
EXMDP	= test
EXLIB	= /usr/local/lib/libmmempool.so

# obj #
OMDP	= source/mempool.c source/example.c 
OOBJ	= source/mempool.c
HEARDER	= source/mempool.h

# phony #
.phony:	build lib cpheader clean

build	: $(EXMDP)
cpheader:
	cp $(HEARDER) /usr/include

lib		: $(EXLIB) cpheader
clean:
	rm -f $(EXMDP) $(EXLIB)

# make #
$(EXMDP) : $(OMDP)
	$(CC) -o $(EXMDP) $(OMDP) $(CFLAGS)

$(EXLIB) : $(OOBJ) $(HEARDER)
	$(CC) $(CFLAGS) $(CCOM) $(CLIB) $(OOBJ) -o $(EXLIB)
