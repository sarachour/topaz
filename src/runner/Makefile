ROOT=$(TOPAZ_ROOT)
INCLUDE=-I$(ROOT)/include
COPTS=-Wall -fPIC -O3
CC=g++
LIB=$(ROOT)/lib
COMMON=$(ROOT)/src/common
COMMON_TASK=$(COMMON)/task
COMMON_MACHINE=$(COMMON)/machine
COMMON_DETECTOR=$(COMMON)/detector
COMMON_SCHEDULER=$(COMMON)/scheduler
COMMON_LOG=$(COMMON)/log
COMMON_EMUL=$(COMMON)/emul
COMMON_UTIL=$(COMMON)/util

DEPOBJS=$(COMMON)/topaz.o \
		$(COMMON)/topaz_pack.o \
		$(COMMON)/bulletin.o \
		$(COMMON_EMUL)/pin_util.o \
		$(COMMON_TASK)/task.o \
		$(COMMON_TASK)/task_spec.o \
		$(COMMON_TASK)/task_specs.o \
		$(COMMON_TASK)/task_spec_args.o \
		$(COMMON_MACHINE)/machine.o \
		$(COMMON_MACHINE)/machine_network.o \
		$(COMMON_SCHEDULER)/scheduler.o \
		$(COMMON_SCHEDULER)/schedule_node.o \
		$(COMMON_SCHEDULER)/task_metric.o \
		$(COMMON_DETECTOR)/checker.o \
		$(COMMON_DETECTOR)/abs_detector.o \
		$(COMMON_DETECTOR)/abs_scar_region_detector.o \
		$(COMMON_DETECTOR)/abs_autoscar_region_detector.o \
		$(COMMON_DETECTOR)/abs_solid_region_detector.o \
		$(COMMON_DETECTOR)/abs_dist_region_detector.o \
		$(COMMON_DETECTOR)/control_system.o \
		$(COMMON_DETECTOR)/distribution.o \
		$(COMMON_DETECTOR)/vector.o \
		$(COMMON_DETECTOR)/stat.o \
		$(COMMON_LOG)/logger.o \
		$(COMMON_LOG)/logger_util.o \
		$(COMMON_LOG)/logger_task.o \
		$(COMMON_LOG)/logger_detector.o \
		$(COMMON_LOG)/logger_detector_util.o \
		$(COMMON_LOG)/timer_util.o \
		$(COMMON_LOG)/timer.o \
		$(COMMON_UTIL)/util.o \
		$(COMMON_UTIL)/relax_lib.o 
		
all:  deps test-libfile libfile

deps:
	cd ../common; make
	
libfile: runner.o worker.o central.o $(DEPOBJS)
	mpic++ -shared -Wl,-soname,libtopaz.so -o libtopaz.so \
	$(DEPOBJS) central.o worker.o runner.o -lrt; 
	mv libtopaz.so $(LIB)/libtopaz.so

test-libfile: runner.o worker.o central.o $(DEPOBJS)
	mpic++ -shared -Wl,-soname,libtesttopaz.so -o libtesttopaz.so \
	$(DEPOBJS) -lrt; 
	mv libtesttopaz.so $(LIB)/libtesttopaz.so
	
%.o: %.cpp
	$(CC) $(COPTS) $(INCLUDE) -o $@ -c $< 
	
clean:
	rm *.o

