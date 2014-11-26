ROOT=$(TOPAZ_ROOT)
INCLUDE=-I$(ROOT)/include
COPTS=-Wall -fPIC -O3
CC=mpic++

all:  scheduler.o schedule_node.o task_metric.o

	
%.o: %.cpp
	$(CC) $(COPTS) $(INCLUDE) -o $@ -c $< 
	
clean:
	rm *.o

