################OPTION###################
CCOMPILE = icc
CPPCOMPILE = icc
COMPILEOPTION = -c -g -O3 -ipo 
# INCLUDEDIR = -I/opt/intel/vtune/analyzer/include 
INCLUDEDIR =
LIBHOME = 
LINK = icc
LINKOPTION = -g -ipo -o ./parasw
LIBS = 
LIBDIRS =  
OBJS = wtime.o ParseFile.o ParallelSWAlgorithm.o ParaSWProcessorSet.o ParaSWProblem.o ParaSWPath.o ParaSWParam.o ParaSWGlobalMemory.o ParallelSW.o Debug.o 
OUTPUT = ./parasw
################OPTION END################
$(OUTPUT):$(OBJS)
#	$(LINK) $(OBJS) /opt/intel/vtune/analyzer/bin/libVtuneApi.so $(LINKOPTION) $(LIBS) $(LIBDIRS) 
	$(LINK) $(OBJS) $(LINKOPTION) $(LIBS) $(LIBDIRS) 

clean: 
	rm -f $(OBJS)
	rm -f $(OUTPUT)
all: clear $(OUTPUT)
.SUFFIXES:
.SUFFIXES:  .c .o .cpp

.cpp.o:
	 $(CPPCOMPILE) -o $*.o $(COMPILEOPTION) $(INCLUDEDIR)  $*.cpp

.c.o:
	$(CCOMPILE) -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $*.c
