#!/bin/bash

EXE_PATH=./bin/Debug/AI_Tetris.exe

H=15
W=20


score=0

for i in $(seq $1); do
	score=$(( $score + $($EXE_PATH play $H $W m $RANDOM | tail | grep -E "[0-9]+\s+pieces." | grep -E -o [0-9]+ )))
done;

echo $score