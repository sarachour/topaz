ROOT=$(TOPAZ_ROOT)
INCLUDE=-I$(ROOT)/include
COPTS=-Wall -fPIC
CC=mpic++

all:  logger_task.o logger_util.o logger.o timer_util.o timer.o logger_detector.o logger_detector_util.o

	
%.o: %.cpp
	$(CC) $(COPTS) $(INCLUDE) -o $@ -c $< 
	
clean:
	rm *.o

