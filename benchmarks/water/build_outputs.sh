#!/bin/bash



#tag="100.12.2"
#params="100 12 2"

tag="100.12.2"
params="100 12 2"




function discard {
	local pcts=$1[@]
	thresh=$2
	mach=$3
	file="pos.$tag.txt"
	
	corrdir="output/correct.TAGS.ltime"
	
	for i in ${!pcts}
	do

		echo "DISCARD: $i"
		#time (echo "c" | ./collect_info.sh discard-$i $params $mach gdt)
		#time (echo "eds" | ./collect_info.sh analyze:discard-$i $params $mach gdt)
		
		outdir="output/discard.TAGS.god.ltime.ldet.ctrl.$i.$mach";
		tpzwater.py "$outdir/plot.$tag.txt" $corrdir/$file $outdir/$file > "$outdir/plot.log.$tag.txt"
		./diff_output.py "$corrdir/$file" "$outdir/$file" > $outdir/summary.$tag.txt

	done

}

function replace {
	local pcts=$1[@]
	thresh=$2
	mach=$3
	
	file="pos.$tag.txt"
	corrdir="output/correct.TAGS.ltime"
	
	for i in ${!pcts}
	do
		echo "REPLACE: $i"
		
		#time (echo "c" | ./collect_info.sh replace-$i $params $mach gdt)
		#time (echo "eds" | ./collect_info.sh analyze:replace-$i $params $mach gdt)
		outdir="output/replace.TAGS.god.ltime.ldet.ctrl.$i.$mach";
		tpzwater.py "$outdir/plot.$tag.txt" $corrdir/$file $outdir/$file > "$outdir/plot.log.$tag.txt"
		./diff_output.py "$corrdir/$file" "$outdir/$file" > $outdir/summary.$tag.txt

		if [ $(echo "$thresh > $i" | bc) -eq 1 ]
		then
			echo "SWISS REPLACE: $i"
			#time (echo "c" | ./collect_info.sh preplace-$i $params $mach gdt)
			#time (echo "eds" | ./collect_info.sh analyze:preplace-$i $params $mach gdt)
			
			outdir="output/replace.TAGS.god.ltime.ldet.ctrl.$i.pa.$mach";
			tpzwater.py "$outdir/plot.$tag.txt" $corrdir/$file $outdir/$file > "$outdir/plot.log.$tag.txt"
			./diff_output.py "$corrdir/$file" "$outdir/$file" > $outdir/summary.$tag.txt
		
		fi
	done

}

function basic {
	mach=$1
	echo "CORRECT"
	file="pos.$tag.txt"
	corrdir="output/correct.TAGS.ltime"
	#time (echo "c" | ./collect_info.sh correct $params $mach t)
	#time (echo "e" | ./collect_info.sh analyze:correct $params $mach t)

	echo "BASIC"
	outdir="output/basic.TAGS.god.ltime.$mach";
	#time (echo "c" | ./collect_info.sh basic $params $mach gt)
	#time (echo "es" | ./collect_info.sh analyze:basic $params $mach gt)
	
	tpzwater.py "$outdir/plot.$tag.txt" $corrdir/$file $outdir/$file > "$outdir/plot.log.$tag.txt"
	./diff_output.py "$corrdir/$file" "$outdir/$file" > $outdir/summary.$tag.txt
}

mach="sram3.cfg"
percents=( 0.10 0.22 0.33 );
min_thresh=0.21;
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
percents=( 0.01 0.022 0.033 );
min_thresh=0.021

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
percents=( 0.001 0.026 0.059 )
min_thresh=0.021

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
