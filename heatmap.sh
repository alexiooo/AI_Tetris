EXE_PATH=./bin/Debug/AI_Tetris.exe

WIDTH="4 8 12 16 20"
HEIGHT="8 12 15"

MONTE=500
TEST_SIZE=$1

for w in $WIDTH; do
	for h in $HEIGHT; do
		$EXE_PATH smart $h $w -10 50 150 -200 -10 $TEST_SIZE $RANDOM
		echo ""
	done
done

#$EXE_PATH benchcarlo $h $w $MONTE $TEST_SIZE $RANDOM
