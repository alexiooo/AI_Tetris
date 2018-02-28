EXE_PATH=./bin/Debug/AI_Tetris.exe

H=15
W=20
TR=-10
EMP=50
CL=150
BL=-200
HDIFF=-10
SMART=0
LIMIT=5000
TEST_SIZE=1
RANGE=10
RANDOM=`date +%s`

for number in {0..TEST_SIZE}; do
echo "itll"
	for num in {0..4};do
		if [ $num -eq 0 ];then
		echo $num
			for num2 in 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1 2 3 4 5 6 7 8 9 10 ;do
			$EXE_PATH s $H $W $num2*$TR $EMP $CL $BL $HDIFF $TEST_SIZE $RANDOM | tail +3 > ./smart.txt
			echo $num2
			done;
		fi
		if [ $num -eq 1 ];then
			for num2 in {0.1 , 0.2 , 0.3 ,0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1,2 ,3 ,4 ,5,6,7 ,8,9,10};do
			$EXE_PATH s $H $W $TR $num2*$EMP $CL $BL $HDIFF $TEST_SIZE $RANDOM | tail +3 > ./smart.txt
			done;
		fi
		if [ $num -eq 2 ];then
			for num2 in {0.1 , 0.2 , 0.3 ,0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1,2 ,3 ,4 ,5,6,7 ,8,9,10};do
			$EXE_PATH s $H $W $TR $EMP $num2*$CL $BL $HDIFF $TEST_SIZE $RANDOM | tail +3 > ./smart.txt
			done;
		fi
		if [ $num -eq 3 ];then
			for num2 in {0.1 , 0.2 , 0.3 ,0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1,2 ,3 ,4 ,5,6,7 ,8,9,10};do
			$EXE_PATH s $H $W $TR $EMP $CL $num2*$BL $HDIFF $TEST_SIZE $RANDOM | tail +3 > ./smart.txt
			done;
		fi
		if [ $num -eq 4 ];then
			for num2 in {0.1 , 0.2 , 0.3 ,0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1,2 ,3 ,4 ,5,6,7 ,8,9,10};do
			$EXE_PATH s $H $W $TR $EMP $CL $BL $num2*$HDIFF $TEST_SIZE $RANDOM  | tail +3 > ./smart.txt
			done;
		fi
	done;
done;