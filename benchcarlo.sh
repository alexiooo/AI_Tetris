EXE_PATH=./bin/Debug/AI_Tetris.exe

H=15
W=20

MONTE=1
LIMIT=5000
TEST_SIZE=1000

RANDOM=`date +%s`

while [[ $MONTE -lt $LIMIT ]]; do
	$EXE_PATH benchcarlo $H $W $MONTE $TEST_SIZE $RANDOM | tail +2
	MONTE=$(($MONTE * 2))
done;