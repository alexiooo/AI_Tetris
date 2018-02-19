EXE_PATH=./bin/Debug/AI_Tetris.exe

H=15
W=20

workers=8

if [[ $# < 3 ]]; then
	echo "Usage:  $0 start stop loops [workers]"
	exit
fi

if [[ $# > 3 ]]; then
	workers=$4
fi

loops=$(( ($3 + $workers - 1) / $workers ))

if [ -e tmp ]; then
	rm -r tmp
fi
mkdir tmp

echo "running from" $1 "to" $2 "with" $((loops*workers)) "loops"
for i in $(seq $workers); do
	$EXE_PATH metric $1 $2 $loops $H $W $RANDOM > tmp/job_$i.csv &
	job[$i]=$!
done

wait ${job[@]}

./readmetric.sh