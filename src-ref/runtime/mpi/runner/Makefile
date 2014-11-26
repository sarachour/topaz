ROOT=../../../..
INCLUDE=$(ROOT)/include
LIB=$(ROOT)/lib
COMMON=$(ROOT)/src/runtime/mpi/common
CLASSIFIER=$(ROOT)/src/runtime/mpi/classifier
COPTS=-Wall -fPIC

all: topaz-lib

topaz-lib: runtime.o $(COMMON)/topaz.o $(COMMON)/timeutil.o $(COMMON)/mem.o $(COMMON)/task.o $(COMMON)/random.o $(COMMON)/fault.o $(COMMON)/relax-lib.o $(COMMON)/timer.o $(COMMON)/logger.o $(COMMON)/task_logger.o $(CLASSIFIER)/outlier-classifier_scar.o $(COMMON)/topaz-classifier.o
	mpic++ -shared -Wl,-soname,libtopaz.so -o libtopaz.so \
	$(COMMON)/topaz.o $(COMMON)/mem.o $(COMMON)/task.o \
	$(COMMON)/random.o $(COMMON)/fault.o $(COMMON)/logger.o $(COMMON)/relax-lib.o \
	$(COMMON)/task_logger.o $(COMMON)/timer.o runtime.o $(CLASSIFIER)/outlier-classifier_scar.o -lsvm -L/usr/lib/libsvm \
	$(COMMON)/topaz-classifier.o $(COMMON)/timeutil.o -lrt; 
	mv libtopaz.so $(LIB)/libtopaz.so
	
$(COMMON)/%.o: $(COMMON)/%.c
	cd $(COMMON); make

$(CLASSIFIER)/%.o: $(CLASSIFIER)/%.c
	cd $(CLASSIFIER); make
	
%.o: %.c
	mpic++ $(COPTS) -I$(INCLUDE) -o $@ -c $<


clean:
	rm *.o $(LIB)/libtopaz.so;
	
