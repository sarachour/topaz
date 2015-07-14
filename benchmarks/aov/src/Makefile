ROOT=../../../
INCLUDE=$(ROOT)/include
LIB=$(ROOT)/lib
CC=mpic++
all: t.inputgen t.blackscholes blackscholes.topaz.c

t.blackscholes: blackscholes.topaz.o
	$(CC) -L$(LIB)  -ltopaz blackscholes.topaz.o -o t.blackscholes
	
%.topaz.c: %.tpz
	tpzcc $<

t.inputgen: inputgen.o $(LIB)/libtopaz.so $(INCLUDE)/topaz.h
	$(CC) -O3 -L$(LIB) inputgen.o -o t.inputgen 
		
%.o: %.c $(INCLUDE)/topaz.h
	$(CC) -O3 -I$(INCLUDE) -o $@ -c $<
	
clean:
	rm -f logs/*.log
	rm -f profile/*.profile
	rm t.*
	rm *.o *.topaz.c
