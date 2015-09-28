# flags #
CC	= g++
CFLAGS	= -g -Wall -I include/

# elf #
EXMDP	= mdpool

# obj #
OMDP	= src/mdp_test.cpp src/mdpool.cpp src/ato.cpp

# phony #
.phony:	build

build	: $(EXMDP)

$(EXMDP) : $(OMDP)
	$(CC) -o $(EXMDP) $(OMDP) $(CFLAGS)