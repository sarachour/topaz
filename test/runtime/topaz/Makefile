ROOT=$(TOPAZ_ROOT)
INCLUDE=-I$(ROOT)/include -I$(UTIL)
LIB=-L$(ROOT)/lib
UTIL=$(ROOT)/test/util
COPTS=-Wall -fPIC
CC=mpic++

all: t.test2 t.test1 t.test3 t.test4

dep:
	cd $(UTIL); make

%.o: %.cpp
	$(CC) $(COPTS) $(INCLUDE) -o $@ -c $< 
	
t.%: %.o dep
	$(CC) $(LIB)  -o $@ $(UTIL)/test_util.o $< -ltopaz
		
clean:
	rm *.o t.*

