#!/bin/bash

function discard {
	local pcts=$1[@]
	thresh=$2
	mach=$3
	file="baboon.ppm"
	corrdir="output/correct.TAGS.ltime"
	for i in ${!pcts}
	do

		echo "DISCARD: $i"
		time (echo "c" | ./collect_info.sh discard-$i baboon $mach gdt)
		time (echo "eds" | ./collect_info.sh analyze:discard-$i baboon $mach gdt)
	
		outdir="output/discard.TAGS.god.ltime.ldet.ctrl.$i.$mach";
		./diff_output.sh $corrdir/$file $outdir/$file $outdir/baboon.diff.ppm &> $outdir/summary.baboon.txt;
		

	done

}

function replace {
	local pcts=$1[@]
	thresh=$2
	mach=$3
	file="baboon.ppm"
	corrdir= "output/correct.TAGS.ltime"
	
	for i in ${!pcts}
	do
		echo "REPLACE: $i"
		time (echo "c" | ./collect_info.sh replace-$i baboon $mach gdt)
		time (echo "eds" | ./collect_info.sh analyze:replace-$i baboon $mach gdt)
		
		outdir="output/replace.TAGS.god.ltime.ldet.ctrl.$i.$mach";
		./diff_output.sh $corrdir/$file $outdir/$file $outdir/baboon.diff.ppm &> $outdir/summary.baboon.txt;
		
		if [ $(echo "$thresh > $i" | bc) -eq 1 ]
		then
			echo "SWISS REPLACE: $i"
			time (echo "c" | ./collect_info.sh preplace-$i baboon $mach gdt)
			time (echo "eds" | ./collect_info.sh analyze:preplace-$i baboon $mach gdt)
			outdir="output/replace.TAGS.god.ltime.ldet.ctrl.$i.pa.$mach";
			./diff_output.sh $corrdir/$file $outdir/$file $outdir/baboon.diff.ppm &> $outdir/summary.baboon.txt;
		
		fi
	done

}

function basic {
	mach=$1
	file="baboon.ppm"
	corrdir="output/correct.TAGS.ltime"
	
	echo "CORRECT"
	time (echo "c" | ./collect_info.sh correct baboon $mach t)
	time (echo "e" | ./collect_info.sh analyze:correct baboon $mach t)
	
	echo "BASIC"
	time (echo "c" | ./collect_info.sh basic baboon $mach gt)
	time (echo "es" | ./collect_info.sh analyze:basic baboon $mach gt)
	outdir="output/basic.TAGS.god.ltime.$mach";
	./diff_output.sh $corrdir/$file $outdir/$file $outdir/baboon.diff.ppm &> $outdir/summary.baboon.txt;
		

}

mach="sram3.cfg"
percents=( 0.04 0.025 0.01 0.08 );
percents=( 0.04 )
min_thresh=0.04;
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
percents=( 0.023 0.05 0.08 );
min_thresh=0.026;

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
