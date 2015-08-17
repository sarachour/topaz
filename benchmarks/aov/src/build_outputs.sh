#!/bin/bash

function discard {
	local pcts=$1[@]
	thresh=$2
	mach=$3
	file="price.64K.txt"
	corrdir="output/correct.TAGS.ltime"
	
	for i in ${!pcts}
	do

		echo "DISCARD: $i"
		time (echo "c" | ./collect_info.sh discard-$i 64K $mach gdt)
		time (echo "eds" | ./collect_info.sh analyze:discard-$i 64K $mach gdt)
		outdir="output/discard.TAGS.god.ltime.ldet.ctrl.$i.$mach";
		./diff_output.py $corrdir/$file $outdir/$file > $outdir/summary.64K.txt

	done

}

function replace {
	local pcts=$1[@]
	thresh=$2
	mach=$3
	file="price.64K.txt"
	corrdir="output/correct.TAGS.ltime"
	
	for i in ${!pcts}
	do
		echo "REPLACE: $i"
		time (echo "c" | ./collect_info.sh replace-$i 64K $mach gdt)
		time (echo "eds" | ./collect_info.sh analyze:replace-$i 64K $mach gdt)
		outdir="output/replace.TAGS.god.ltime.ldet.ctrl.$i.$mach";
		./diff_output.py $corrdir/$file $outdir/$file > $outdir/summary.64K.txt

		if [ $(echo "$thresh > $i" | bc) -eq 1 ]
		then
			echo "SWISS REPLACE: $i"
			time (echo "c" | ./collect_info.sh preplace-$i 64K $mach gdt)
			time (echo "eds" | ./collect_info.sh analyze:preplace-$i 64K $mach gdt)
			outdir="output/replace.TAGS.god.ltime.ldet.ctrl.$i.pa.$mach";
			./diff_output.py $corrdir/$file $outdir/$file > $outdir/summary.64K.txt

		fi
	done

}

function basic {
	file="price.64K.txt"
	corrdir="output/correct.TAGS.ltime"
	
	mach=$1
	echo "CORRECT"
	#time (echo "c" | ./collect_info.sh correct 64K $mach t)
	#time (echo "e" | ./collect_info.sh analyze:correct 64K $mach t)

	echo "BASIC"
	time (echo "c" | ./collect_info.sh basic 64K $mach gt)
	time (echo "es" | ./collect_info.sh analyze:basic 64K $mach gt)
	outdir="output/basic.TAGS.god.ltime.$mach";
	./diff_output.py $corrdir/$file $outdir/$file > $outdir/summary.64K.txt

}

mach="sram3.cfg"
percents=( 0.13 0.17 0.08 );
min_thresh=0.12;
echo "MACHINE: $mach"
echo "What do you want to calculate: [b:basic, r:replacement, d: discard]"
read choice
if [[ $choice == *b* ]]
then
	basic $mach;
fi
if [[ $choice == *r* ]]
then
	replace percents $min_thresh $mach
fi
if [[ $choice == *d* ]]
then
	discard percents $min_thresh $mach
fi

mach="sram4.cfg"
percents=( 0.01 0.02 0.05);
min_thresh=0.02;

echo "MACHINE: $mach"
echo "What do you want to calculate: [b:basic, r:replacement, d: discard]"
read choice
if [[ $choice == *b* ]]
then
	basic $mach;
fi
if [[ $choice == *r* ]]
then
	replace percents $min_thresh $mach
fi
if [[ $choice == *d* ]]
then
	discard percents $min_thresh $mach
fi

mach="sram5.cfg"
percents=( 0.0016 0.015 0.030 );
percents=( 0.030 )
min_thresh=0.0136;

echo "MACHINE: $mach"
echo "What do you want to calculate: [b:basic, r:replacement, d: discard]"
read choice
if [[ $choice == *b* ]]
then
	basic $mach;
fi
if [[ $choice == *r* ]]
then
	replace percents $min_thresh $mach
fi
if [[ $choice == *d* ]]
then
	discard percents $min_thresh $mach
fi
