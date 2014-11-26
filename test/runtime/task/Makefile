ROOT=$(TOPAZ_ROOT)
INCLUDE=-I$(ROOT)/include -I$(UTIL)
LIB=-L$(ROOT)/lib
UTIL=$(ROOT)/test/util
COPTS=-Wall -fPIC
CC=mpic++

all: test_task

dep:
	cd $(UTIL); make
	
test_task: test_task.o test.o dep
	$(CC) $(LIB)  -o test test_task.o $(UTIL)/test_util.o test.o -ltopaz
	
%.o: %.cpp
	$(CC) $(COPTS) $(INCLUDE) -o $@ -c $< 
	
clean:
	rm *.o

