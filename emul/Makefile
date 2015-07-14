PIN_DIR=siact/tool/pin-2.12-58423-gcc.4.4.7-linux/source/tools/SiAct

all: pin

pin: $(PIN_DIR)/siact.cpp $(PIN_DIR)/*.H $(PIN_DIR)/include/*.H
	cd $(PIN_DIR); make clean; make
	cp $(PIN_DIR)/include/* $(TOPAZ_ROOT)/include
	cp $(PIN_DIR)/include/*.cpp $(TOPAZ_ROOT)/src/common/emul
	cp $(PIN_DIR)/obj-intel64/siact.so $(TOPAZ_ROOT)/lib
	
clean:
	cd $(PIN_DIR); make clean
