# Makefile for streamcluster

PREFIX=${PARSECDIR}/pkgs/kernels/streamcluster/inst/${PARSECPLAT}

TARGET=streamcluster
OBJS=streamcluster.o util.o

TOPAZ_INCLUDE=$(TOPAZ_ROOT)/include
TOPAZ_LIB=$(TOPAZ_ROOT)/lib
CXXFLAGS := $(CXXFLAGS) -I$(TOPAZ_INCLUDE) -g
LIBS := $(LIBS) -ltopaz -lglpk
LDFLAGS := $(LDFLAGS) -L$(TOPAZ_LIB)

all: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o $(TARGET)

install:
	mkdir -p $(PREFIX)/bin
	cp -f $(TARGET) $(PREFIX)/bin/$(TARGET)

