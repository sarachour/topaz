all: test

test: test.o instrument.o
	gcc test.o instrument.o -o test

test.o: test.c instrument.h
	gcc -c -I. test.c

instrument.o: instrument.c instrument.h
	gcc -c -I. instrument.c
	
clean : 
	rm -rf *.o test
