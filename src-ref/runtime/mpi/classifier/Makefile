ROOT=$(TOPAZ_ROOT)
INCLUDE=-I$(ROOT)/include
COPTS=-Wall -fPIC

#linking flags: -lsvm -L/usr/lib/libsvm
all:  outlier-classifier_scar.o


svm-test: svm-example.o
	mpic++ -o svm-test  svm-example.o -lsvm -L/usr/lib/libsvm
	
%.o: %.c
	mpic++ $(COPTS) $(INCLUDE) -I/usr/include/libsvm -o $@ -c $< 
	
clean:
	rm *.o
