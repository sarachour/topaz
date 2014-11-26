ROOT=$(TOPAZ_ROOT)
INCLUDE=-I$(ROOT)/include -I$(UTIL)
LIB=-L$(ROOT)/lib
UTIL=$(ROOT)/test/util
COPTS=-Wall -fPIC
CC=mpic++

all: test_dist

dep:
	cd $(UTIL); make
	
test_dist: test_dist.o test.o dep
	$(CC)  -o test $(LIB) test_dist.o $(UTIL)/test_util.o test.o -ltesttopaz -lglpk
	
%.o: %.cpp
	$(CC) $(COPTS) $(INCLUDE) -o $@ -c $< 
	
clean:
	rm *.o

