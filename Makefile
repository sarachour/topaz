all: makeall 


makeall: 
	cd src; make
	cd benchmarks; make
	cd emul; make
	
clean:
	cd src; make clean
	cd benchmarks; make clean
	cd emul; make clean
