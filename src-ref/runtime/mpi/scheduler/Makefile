ROOT=../../../..
INCLUDE=$(ROOT)/include
COPTS=-Wall -fPIC

all: scheduler.o

%.o: %.c
	mpicc $(COPTS) -I$(INCLUDE) -o $@ -c $< 
	
clean:
	rm *.o
