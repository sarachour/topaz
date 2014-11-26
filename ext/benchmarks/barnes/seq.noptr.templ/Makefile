ROOT=../..
INCLUDE=$(ROOT)/include
LIB=$(ROOT)/lib
CC=mpic++
all: barnes

barnes: extfcts.o stat.o vector.o matrix.o node.o set.o parms.o nbody.o nbody_maketree.o main.o globals.o
	$(CC) extfcts.o stat.o vector.o matrix.o node.o set.o parms.o nbody.o nbody_maketree.o globals.o main.o  -o barnes 
	
%.o: %.cpp 
	$(CC) -o $@ -c $<
	
%.o: %.c 
	$(CC) -o $@ -c $<
		
clean:
	rm *.o barnes
