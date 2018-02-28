EXE_PATH=./bin/Debug/AI_Tetris.exe

H=15
W=20

TR=-1
EMP=5
CL=15
BL=-20
HDIFF=-1

TEST_SIZE=500
RANGE="1 2 5 8 10 25 50 75 100"

RANDOM=`date +%s`

if [[ $# < 1 ]]; then
	echo "Usage:  $0 [outputfile]"
	exit
fi

if [ -e tmp ]; then
	rm -r tmp
fi
mkdir tmp

for mod in $RANGE ;do
	$EXE_PATH s $H $W $(($mod*$TR/10)) $EMP $CL $BL $HDIFF $TEST_SIZE $RANDOM | head -n2 >> tmp/smart_0.csv &
	
	$EXE_PATH s $H $W $TR $(($mod*$EMP/10)) $CL $BL $HDIFF $TEST_SIZE $RANDOM | head -n2 >> tmp/smart_1.csv &
	
	$EXE_PATH s $H $W $TR $EMP $(($mod*$CL/10)) $BL $HDIFF $TEST_SIZE $RANDOM | head -n2 >> tmp/smart_2.csv &
	
	$EXE_PATH s $H $W $TR $EMP $CL $(($mod*$BL/10)) $HDIFF $TEST_SIZE $RANDOM | head -n2 >> tmp/smart_3.csv &
	
	$EXE_PATH s $H $W $TR $EMP $CL $BL $(($mod*$HDIFF/10)) $TEST_SIZE $RANDOM | head -n2 >> tmp/smart_4.csv &
done

wait $(jobs -p)

cat tmp/smart_*.csv > $1
cat $1 | awk 'NR%2==0'
