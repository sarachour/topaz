COPTS=-Wall -fPIC

all: water

water: extfcts.o stat.o water.o
	g++ -o water extfcts.o stat.o water.o
	
%.o: %.c $(INCLUDE)/topaz.h
	gcc $(COPTS) -I$(INCLUDE) -o $@ -c $<
	
clean:
	rm *.o water
