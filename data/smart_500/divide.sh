#!/bin/bash

TR=-10
EMP=50
CL=150
BL=-200
HDIFF=-10

rm div_*.csv div_*.tmp

cat $1 | while read line; do { 
	read line2;
	n=$(echo $line2 | awk -F',' "{
		if (\$1==$TR && \$2==$EMP && \$3==$CL && \$4==$BL && \$5==$HDIFF)
			print \"0\"
		else if (\$2==$EMP && \$3==$CL && \$4==$BL && \$5==$HDIFF)
			print \$1 \";1\"
		else if (\$1==$TR && \$3==$CL && \$4==$BL && \$5==$HDIFF)
			print \$2 \";2\"
		else if (\$1==$TR && \$2==$EMP && \$4==$BL && \$5==$HDIFF)
			print \$3 \";3\"
		else if (\$1==$TR && \$2==$EMP && \$3==$CL && \$5==$HDIFF)
			print \$4 \";4\"
		else if (\$1==$TR && \$2==$EMP && \$3==$CL && \$4==$BL)
			print \$5 \";5\"
	}");
	
	if [[ "x$n" == "x0" ]]; then
		echo $line | sed 's/,/\n/g' | sed "s/^/$TR,/" >> div_1.csv; 
		echo $TR , $(echo $line2 | awk -F',' '{print $7/$6}') >> div_1_avg.csv.tmp
		
		echo $line | sed 's/,/\n/g' | sed "s/^/$EMP,/" >> div_2.csv; 
		echo $EMP , $(echo $line2 | awk -F',' '{print $7/$6}') >> div_2_avg.csv.tmp
		
		echo $line | sed 's/,/\n/g' | sed "s/^/$CL,/" >> div_3.csv; 
		echo $CL , $(echo $line2 | awk -F',' '{print $7/$6}') >> div_3_avg.csv.tmp
		
		echo $line | sed 's/,/\n/g' | sed "s/^/$BL,/" >> div_4.csv; 
		echo $BL , $(echo $line2 | awk -F',' '{print $7/$6}') >> div_4_avg.csv.tmp
		
		echo $line | sed 's/,/\n/g' | sed "s/^/$HDIFF,/" >> div_5.csv; 
		echo $HDIFF , $(echo $line2 | awk -F',' '{print $7/$6}') >> div_5_avg.csv.tmp
	else
		x=$(echo $n | awk -F';' '{print $1}')
		f=$(echo $n | awk -F';' '{print $2}')
		echo $line | sed 's/,/\n/g' | sed "s/^/$x,/" >> div_$f.csv; 
		echo $x , $(echo $line2 | awk -F',' '{print $7/$6}') >> div_${f}_avg.csv.tmp
	fi
	}
done

for f in 1 2 3 4 5; do
	cat div_${f}_avg.csv.tmp | sort -n > div_${f}_avg.csv
done

rm div_*.tmp
