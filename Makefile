# flags #
CC		= gcc
CFLAGS	= -g -Wall -std=c99

# elf #
EXMDP	= test

# obj #
OMDP	= source/mempool.c source/example.c 

# phony #
.phony:	build

build	: $(EXMDP)

$(EXMDP) : $(OMDP)
	$(CC) -o $(EXMDP) $(OMDP) $(CFLAGS)
