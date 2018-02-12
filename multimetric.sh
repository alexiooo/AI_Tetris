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

cat $(for i in $(seq $workers); do echo tmp/job_$i.csv; done) | sort |
sed -E -e '/^([0-9]+),([0-9]+)/{
	   s/^([0-9]+(,[0-9]+){3}),([0-9]+)/\1,=\3/
       $!{          # append the next line when not on the last line
	     :loop
		 N
		 
         s/^([0-9]+(,[0-9]+){3}),(=[0-9]+(\+[0-9]+)*)\s+\1,([0-9]+)/\1,\3\+\5/
                    # now test for a successful substitution, otherwise
                    #+  unpaired "a test" lines would be mis-handled
         t loop  # branch_on_substitute (goto label :sub-yes)
         :sub-not   # a label (not essential; here to self document)
                    # if no substituion, print only the first line
         P          # pattern_first_line_print
         D          # pattern_ltrunc(line+nl)_top/cycle
       }    
     }'