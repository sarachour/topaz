ROOT=$(TOPAZ_ROOT)
INCLUDE=-I$(ROOT)/include
COPTS=-Wall -fPIC -O3
CC=mpic++

all:  checker.o \
	  vector.o \
	  distribution.o \
	  abs_detector.o \
	  abs_solid_region_detector.o \
	  abs_scar_region_detector.o \
	  abs_autoscar_region_detector.o \
	  abs_dist_region_detector.o \
	  control_system.o \
	  stat.cpp
	
%.o: %.cpp
	$(CC) $(COPTS) $(INCLUDE) -o $@ -c $< 
	
clean:
	rm *.o

