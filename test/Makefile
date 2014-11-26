ROOT=$(TOPAZ_ROOT)
INCLUDE=-I$(ROOT)/include
COPTS=-Wall -fPIC
CC=mpic++

all: subdirs

subdirs:
	cd compiler; make
	cd runtime; make

clean:
	cd compiler; make clean
	cd runtime; make clean

