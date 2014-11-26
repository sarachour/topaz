all: test test2 test3

test3: test3.o
	mpicc -o test3 test3.o

test3.o: test3.c
	mpicc -c test3.c

test2: test2.o
	mpicc -o test2 test2.o

test2.o: test2.c
	mpicc -c test2.c

test: test.o
	mpicc -o test test.o

test.o: test.c
	mpicc -c test.c

clean:
	rm -rf *.o test2 test test3
