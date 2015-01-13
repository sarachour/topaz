ROOT=../../..
INCLUDE=$(ROOT)/include
UTIL=$(ROOT)/benchmarks/examples/util
UTIL_OBJS=$(UTIL)/rand.o $(UTIL)/dist.o
CURR=$(pwd)
LIB=$(ROOT)/lib
CC=mpic++

all: ex1d

ex1d: main.topaz.o
	cd $(UTIL); make
	$(CC) -L$(LIB) -ltopaz main.topaz.o $(UTIL_OBJS) -o ex2d
	
%.topaz.c: %.tpz
	tpzcc $<

%.o: %.c 
	$(CC) -O3 -I$(INCLUDE) -I$(UTIL) -o $@ -c $<
	
clean:
	rm -f logs/*.log
	rm -f profile/*.profile
	rm *.o *.topaz.c
	rm ex2d
