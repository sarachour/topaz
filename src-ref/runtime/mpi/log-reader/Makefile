ROOT=../../../..
INCLUDE=$(ROOT)/include
UTIL=$(ROOT)/util
COMMON=$(ROOT)/src/runtime/mpi/common
CLASSIFIER=$(ROOT)/src/runtime/mpi/classifier
COPTS=-Wall -fPIC

all: log-reader log-classifier

log-classifier: log-classifier.o $(COMMON)/logger.o $(CLASSIFIER)/outlier-classifier_scar.o
	mpic++ -o log-classifier $(COMMON)/logger.o $(CLASSIFIER)/outlier-classifier_scar.o log-classifier.o;
	mv log-classifier $(UTIL)/cllog
	
log-reader: log-reader.o $(COMMON)/logger.o
	mpic++ -o log-reader $(COMMON)/logger.o log-reader.o;
	mv log-reader $(UTIL)/rdlog

%.o: %.c
	mpic++ $(COPTS) -I$(INCLUDE) -o $@ -c $<
	
$(COMMON)/%.o: $(COMMON)/%.c
	cd $(COMMON); make

$(CLASSIFIER)/%.o: $(CLASSIFIER)/%.c
	cd $(CLASSIFIER); make
	
clean:
	rm *.o
