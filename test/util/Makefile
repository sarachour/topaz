ROOT=$(TOPAZ_ROOT)
INCLUDE=-I$(ROOT)/include
LIB=-L$(ROOT)/lib
COPTS=-Wall -fPIC
CC=mpic++

all: test_util.o

%.o: %.cpp
	$(CC) $(COPTS) $(INCLUDE) -o $@ -c $< 
	
clean:
	rm *.o

