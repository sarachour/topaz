ROOT=../../../..
INCLUDE=$(ROOT)/include
COPTS=-Wall -fPIC

all: topaz.o mem.o task.o fault.o timer.o random.o logger.o topaz-classifier.o task_logger.o timeutil.o relax-lib.o

timeutil.o: timeutil.c
	mpic++ $(COPTS) -I$(INCLUDE) -o $@ -c $< 
	
%.o: %.c
	mpicc $(COPTS) -I$(INCLUDE) -o $@ -c $< 
	
clean:
	rm *.o
