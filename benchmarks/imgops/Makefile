ROOT=../..
INCLUDE=$(ROOT)/include
LIB=$(ROOT)/lib
CC=mpic++
all: t.imgops ops.topaz.c


%.topaz.c: %.tpz
	tpzcc $<


t.imgops: image.o ops.topaz.o routine.o 
	$(CC) -L$(LIB) -ltopaz routine.o image.o ops.topaz.o -o t.imgops 
	
%.o: %.c $(INCLUDE)/topaz.h
	$(CC) -I$(INCLUDE) -o $@ -c $<
	
clean:
	rm *.o t.*
	rm logs/*.log
	rm profile/*.profile
