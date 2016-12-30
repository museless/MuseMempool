# flags #
CC		= gcc
CFLAGS	= -g -Wall -std=c99 -D_DEFAULT_SOURCE
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
.phony:	build lib cpheader

build	: $(EXMDP)
cpheader:
	cp $(HEARDER) /usr/include

lib		: $(EXLIB) cpheader

$(EXMDP) : $(OMDP)
	$(CC) -o $(EXMDP) $(OMDP) $(CFLAGS)

$(EXLIB) : $(OOBJ) $(HEARDER)
	$(CC) $(CFLAGS) $(CCOM) $(CLIB) $(OOBJ) -o $(EXLIB)
