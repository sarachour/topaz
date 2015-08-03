# Makefile for streamcluster

PREFIX=${PARSECDIR}/pkgs/kernels/streamcluster/inst/${PARSECPLAT}

TARGET=streamcluster
OBJS=streamcluster.topaz.o util.o

TOPAZ_INCLUDE=$(TOPAZ_ROOT)/include
TOPAZ_LIB=$(TOPAZ_ROOT)/lib
CXXFLAGS := $(CXXFLAGS) -I$(TOPAZ_INCLUDE) -g
LIBS := $(LIBS) -ltopaz
LDFLAGS := $(LDFLAGS) -L$(TOPAZ_LIB)

all: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

%.topaz.c: %.tpz
	tpzcc $<
	
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

%.topaz.o : %.topaz.c
	$(CXX) $(CXXFLAGS) -c $<
	
%.o : %.c
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o *.topaz.c $(TARGET) 

install:
	mkdir -p $(PREFIX)/bin
	cp -f $(TARGET) $(PREFIX)/bin/$(TARGET)

