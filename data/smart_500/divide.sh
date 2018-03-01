#!/bin/bash

TR=-10
EMP=50
CL=150
BL=-200
HDIFF=-10

rm div_*.csv

cat $1 | while read line; do { 
	read line2;
	n=$(echo $line2 | awk -F',' "{
		if (\$1==$TR && \$2==$EMP && \$3==$CL && \$4==$BL && \$5==$HDIFF)
			print \"-;0\"
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
		else
			print \$0
	}");
	x=$(echo $n | awk -F';' '{print $1}')
	f=$(echo $n | awk -F';' '{print $2}')
	echo $line | sed 's/,/\n/g' | sed "s/^/$x,/" >> div_$f.csv; 
	echo $x , $(echo $line2 | awk -F',' '{print $7/$6}') >> div_${f}_avg.csv
	}
done
