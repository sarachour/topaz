ROOT=$(TOPAZ_ROOT)
INCLUDE=-I$(ROOT)/include
COPTS=-Wall -fPIC
CC=mpic++

all:  *.o

	
%.o: %.cpp
	$(CC) $(COPTS) $(INCLUDE) -o $@ -c $< 
	
clean:
	rm *.o

