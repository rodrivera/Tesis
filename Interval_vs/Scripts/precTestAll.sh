#!/bin/bash
AUX=""
#AUX="../"

qFile="$1"
queries="$AUX""data/queries/""$qFile"".txt"

if [ ! -f "$queries" ]; then
    echo "Queries file not found!"
    echo "Usage: precTestAll.sh search_window_SIZE"
    echo "with size between 0 and 100 or rrr"
    exit
fi

declare -a SUFFIXES=("0" "2" "4" "6" "8")
for N in "${SUFFIXES[@]}"
do
	intervals="$AUX""data/intervals/800k/""$N"".txt"
	outR="$AUX""data/output/R/""$qFile""-""$N"".txt"
	outI="$AUX""data/output/I/""$qFile""-""$N"".txt"
	outS="$AUX""data/output/S/""$qFile""-""$N"".txt"
	outC1="$AUX""data/output/C1/""$qFile""-""$N"".txt"
	echo "----- Intervals precision = ""$N"" and QueryWindow size = ""$qFile"".. -----"
	#echo "Executing.. FNR-tree with ""$N"" objects.."
	./"$AUX"rtest.out $intervals $queries $outR
	#echo "Executing..        X with ""$N"" objects.."
	./"$AUX"itest.out $intervals $queries $outI
	#echo "Executing..  Schmidt with ""$N"" objects.."
	./"$AUX"stest.out $intervals $queries $outS
	#echo "Executing..  Compressed I with ""$N"" objects.."
	./"$AUX"c1test.out $intervals $queries $outC1 $((10**$N))

	echo ""; echo "Checking differences between Output files.."
	if diff $outR $outI >/dev/null; then
		if diff $outR $outS >/dev/null; then
			if diff $outR $outC1 >/dev/null; then
				printf "Passed\n"
			else
				printf "¡¡ Failed (C1) !!\n"
				sdiff $outR $outC1
				exit
			fi
		else
			printf "¡¡ Failed (Schmidt) !!\n"
			sdiff $outR $outS
			exit
		fi
	else
		printf "¡¡ Failed !!\n"
		sdiff $outR $outI
		exit
	fi
	echo "-------------------------------------------"; echo ""
done