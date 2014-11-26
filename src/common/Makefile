ROOT=$(TOPAZ_ROOT)
INCLUDE=-I$(ROOT)/include
COPTS=-Wall -fPIC -O3
CC=mpic++

all:  topaz.o topaz_pack.o subdirs

subdirs:
	cd task; make
	cd machine; make
	cd scheduler; make
	cd detector; make
	cd util; make
	cd log; make
	cd emul; make

%.o: %.cpp
	$(CC) $(COPTS) $(INCLUDE) -o $@ -c $< 
	
clean:
	rm -f *.o
	cd task; make clean
	cd machine; make clean
	cd scheduler; make clean
	cd detector; make clean
	cd util; make clean
	cd log; make clean
	cd emul; make clean
