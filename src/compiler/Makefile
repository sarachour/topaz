all: tpzcc

tpzcc: 
	rm -f $(TOPAZ_ROOT)/util/tpzcc.native
	ocamlbuild -lib str tpzcc.native && \
	ln -s $(PWD)/_build/tpzcc.native $(TOPAZ_ROOT)/util/tpzcc.native && \
	chmod +x $(TOPAZ_ROOT)/util/tpzcc.native \
	
clean:
	rm -r _build tpzcc.native $(TOPAZ_ROOT)/util/tpzcc.native *.o
