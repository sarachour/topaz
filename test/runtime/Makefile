ROOT=$(TOPAZ_ROOT)
INCLUDE=-I$(ROOT)/include
COPTS=-Wall -fPIC
CC=mpic++

all: subdirs

subdirs:
	cd task; make
	cd runner; make
	cd machine; make
	cd scheduler; make
	cd detector; make


clean:
	cd task; make clean
	cd runner; make clean
	cd machine; make clean
	cd scheduler; make clean
	cd detector; make clean
