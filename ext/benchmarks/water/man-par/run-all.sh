

if [ "X$1" == "X" ]; then
    PROC=`grep -c "processor" /proc/cpuinfo`
else
    PROC=$1
fi

echo "Setting the number of threads = $PROC"

#for X in `echo S P PI PIV PIP PIPV PIR PIPR`; do
for X in `echo S P PR PIPR`; do
    echo "execute water-$X: "; 
    cat LWI5 | ./water-$X $PROC > OUT-$X
#    cat OUT-$X
    cat OUT-$X| egrep "( 12 |Total)"  ; 
    cat OUT-$X| egrep " 12 " > RES-$X ; 
    X=`cat OUT-$X | egrep "Total" | awk '{ print $4; }'`
    Y=`cat OUT-S | egrep "Total" | awk '{ print $4; }'`
    echo $X $Y | awk '{ printf "Speedup: %10.4f\n", ($2/1.0/$1) }'
    echo;
done

diff RES-S RES-PIPR
rm OUT-* RES-*