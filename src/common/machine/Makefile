ROOT=$(TOPAZ_ROOT)
INCLUDE=-I$(ROOT)/include
COPTS=-Wall -fPIC -O3
CC=mpic++

all:  machine.o machine_network.o

	
%.o: %.cpp
	$(CC) $(COPTS) $(INCLUDE) -o $@ -c $< 
	
clean:
	rm *.o

