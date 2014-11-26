ROOT=$(TOPAZ_ROOT)
INCLUDE=-I$(ROOT)/include
LIB=$(ROOT)/lib
CC=mpic++
all: t.water water.topaz.c

t.water: water.topaz.o water_util.o extfcts.o stat.o 
	$(CC)  -L$(LIB)   -ltopaz -lglpk extfcts.o stat.o water_util.o water.topaz.o  -o t.water 
	
%.o: %.cpp
	$(CC) $(INCLUDE) -O3 -o $@ -c $<

%.o: %.c
	$(CC) $(INCLUDE) -O3 -o $@ -c $<
	
	
%.topaz.c: %.tpz
	tpzcc $< -c
	
clean:
	rm -f t.*
	rm -f *.o *.topaz.c 
	rm -f logs/*.log
	rm -f profile/*.profile
