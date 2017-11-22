#!/bin/bash
AUX=""
#AUX="../"

qFile="$1"
queries="$AUX""data/queries/""$qFile"".txt"

if [ ! -f "$queries" ]; then
    echo "Queries file not found!"
    echo "Usage: testAll.sh search_window_SIZE"
    echo "with size between 0 and 100"
    exit
fi

declare -a SUFFIXES=("10N5000" "10N10000" "10N25000" "10N50000" "10N100000")
for N in "${SUFFIXES[@]}"
do
	intervals="$AUX""data/intervals/""$N"".txt"
	outR="$AUX""data/output/R/""$qFile""-""$N"".txt"
	outI="$AUX""data/output/I/""$qFile""-""$N"".txt"
	outS="$AUX""data/output/S/""$qFile""-""$N"".txt"
	echo "----- Intervals size = ""$N"" and QueryWindow size = ""$qFile"".. -----"
	#echo "Executing.. FNR-tree with ""$N"" objects.."
	./"$AUX"rtest.out $intervals $queries $outR
	#echo "Executing..        X with ""$N"" objects.."
	./"$AUX"itest.out $intervals $queries $outI
	#echo "Executing..  Schmidt with ""$N"" objects.."
	./"$AUX"stest.out $intervals $queries $outS

	echo ""; echo "Checking differences between Output files.."
	if diff $outR $outI >/dev/null; then
		if diff $outR $outS >/dev/null; then
			printf "Passed\n"
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
