EXE_PATH=./bin/Debug/AI_Tetris.exe

H=15
W=20

TR=-10
EMP=50
CL=150
BL=-200
HDIFF=-10

TEST_SIZE=500
RANGE="1 2 5 8 10 25 50 75 100"

RANDOM=`date +%s`

if [ -e tmp ]; then
	rm -r tmp
fi
mkdir tmp

for mod in $RANGE ;do {
	$EXE_PATH s $H $W $(($mod*$TR/10)) $EMP $CL $BL $HDIFF $TEST_SIZE $RANDOM
	$EXE_PATH s $H $W $TR $(($mod*$EMP/10)) $CL $BL $HDIFF $TEST_SIZE $RANDOM
	$EXE_PATH s $H $W $TR $EMP $(($mod*$CL/10)) $BL $HDIFF $TEST_SIZE $RANDOM
	$EXE_PATH s $H $W $TR $EMP $CL $(($mod*$BL/10)) $HDIFF $TEST_SIZE $RANDOM
	$EXE_PATH s $H $W $TR $EMP $CL $BL $(($mod*$HDIFF/10)) $TEST_SIZE $RANDOM
} &
done | awk 'NR%4==2 || NR%4==1'
