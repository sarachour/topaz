ROOT=../../../..
INCLUDE=$(ROOT)/include
LIB=$(ROOT)/lib
CC=mpic++
CARGS=-O3 
all: t.barnes nbody_sim.topaz.c 


t.barnes: extfcts.o stat.o vector.o matrix.o node.o set.o parms.o nbody.o nbody_sim.topaz.o nbody.o nbody_maketree.o globals.o $(LIB)/libtopaz.so $(INCLUDE)/topaz.h
	$(CC) $(CARGS) -L$(LIB) -ltopaz extfcts.o stat.o vector.o matrix.o node.o set.o parms.o nbody_sim.topaz.o nbody.o nbody_maketree.o globals.o  -o t.barnes 

%.topaz.c: %.tpz
	tpzcc $< -c

%.o: %.cpp $(INCLUDE)/topaz.h
	$(CC) $(CARGS) -I$(INCLUDE) -o $@ -c $<
	
%.o: %.c $(INCLUDE)/topaz.h
	$(CC) $(CARGS) -I$(INCLUDE) -o $@ -c $<
		
clean:
	rm -f logs/*.log 
	rm -f profile/*.profile; 
	rm -f *.o barnes t.* *.topaz.c
