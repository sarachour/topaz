ROOT=../..
INCLUDE=$(ROOT)/include
LIB=$(ROOT)/lib
CC=mpic++
all: rand.o dist.o
	
%.topaz.c: %.tpz
	tpzcc $<

%.o: %.c $(INCLUDE)/topaz.h
	$(CC) -O3 -I$(INCLUDE) -o $@ -c $<
	
clean:
	rm -f logs/*.log
	rm -f profile/*.profile
	rm *.o *.topaz.c
