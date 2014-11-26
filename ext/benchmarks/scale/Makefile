
all: scale scale-instr

scale-instr: scale-instr.o main-instr.o image/image.o INSTRUMENT/instrument.o
	gcc image/image.o INSTRUMENT/instrument.o scale-instr.o main-instr.o -lm -o scale-instr

scale: scale.o main.o image/image.o
	gcc image/image.o scale.o main.o -lm -o scale

scale-instr.o: scale-instr.c scale.h INSTRUMENT/instrument.h
	gcc -c -I. -Iimage -IINSTRUMENT scale-instr.c -o scale-instr.o 

main-instr.o: image/image.h scale.h main-instr.cpp INSTRUMENT/instrument.h
	g++ -c -I. -Iimage -IINSTRUMENT main-instr.cpp -o main-instr.o

scale.o: scale.c scale.h
	gcc -c -I. -Iimage scale.c -o scale.o 

main.o: image/image.h scale.h main.cpp
	g++ -c -I. -Iimage main.cpp -o main.o

image/image.o: image/image.c image/image.h
	cd image; make

INSTRUMENT/instrument.o: INSTRUMENT/instrument.c
	cd INSTRUMENT; make
clean:
	rm *.o scale scale-instr
	cd image; make clean
