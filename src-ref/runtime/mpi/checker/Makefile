ROOT=../../../..
INCLUDE=$(ROOT)/include
COPTS=-Wall -fPIC

all: checker.o

%.o: %.c
	mpicc $(COPTS) -I$(INCLUDE) -o $@ -c $< 
	
clean:
	rm *.o
