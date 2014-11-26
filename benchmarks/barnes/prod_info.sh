OP=$1

if [ "$OP" == "correct" ]
then
	rm -rf output/correct
	./collect_info.sh correct 16K 
fi
if [ "$OP" == "basic" ]
then
	rm -rf output/basic
	./collect_info.sh godmode 16K sram3.rand1.cfg 
	./collect_info.sh godmode 16K sram3.rand2.cfg 
	./collect_info.sh godmode 16K sram3.rand3.cfg 
	./collect_info.sh godmode 16K sram3.rand4.cfg 
	./collect_info.sh godmode 16K sram3.rand5.cfg 
	./collect_info.sh godmode 16K sram3.rand6.cfg 
	./collect_info.sh godmode 16K sram3.rand7.cfg 
	./collect_info.sh godmode 16K sram3.rand8.cfg 
fi

if [ "$OP" == "detect-c" ]
then
	CFG="c"
	./collect_info.sh godmode-detect 16K sram3.rand1.cfg $CFG
	./collect_info.sh godmode-detect 16K sram3.rand2.cfg $CFG
	./collect_info.sh godmode-detect 16K sram3.rand3.cfg $CFG
	./collect_info.sh godmode-detect 16K sram3.rand4.cfg $CFG
	./collect_info.sh godmode-detect 16K sram3.rand5.cfg $CFG
	./collect_info.sh godmode-detect 16K sram3.rand6.cfg $CFG
	./collect_info.sh godmode-detect 16K sram3.rand7.cfg $CFG
	./collect_info.sh godmode-detect 16K sram3.rand8.cfg $CFG
fi

if [ "$OP" == "detect-m" ]
then
	CFG="m"
	./collect_info.sh godmode-detect 16K sram3.rand1.cfg $CFG
	./collect_info.sh godmode-detect 16K sram3.rand2.cfg $CFG
	./collect_info.sh godmode-detect 16K sram3.rand3.cfg $CFG
	./collect_info.sh godmode-detect 16K sram3.rand4.cfg $CFG
	./collect_info.sh godmode-detect 16K sram3.rand5.cfg $CFG
	./collect_info.sh godmode-detect 16K sram3.rand6.cfg $CFG
	./collect_info.sh godmode-detect 16K sram3.rand7.cfg $CFG
	./collect_info.sh godmode-detect 16K sram3.rand8.cfg $CFG
fi

if [ "$OP" == "consolidate" ]
then
	cat output/godmode-detect/emul.c.16K.sram3.rand* > output/godmode-detect/emul.c.16K.sram3.CONSOLIDATE.txt
	cat output/godmode-detect/emul.m.16K.sram3.rand* > output/godmode-detect/emul.m.16K.sram3.CONSOLIDATE.txt
	cat output/godmode/emul.16K.sram3.rand* > output/godmode/emul.16K.sram3.CONSOLIDATE.txt


fi
