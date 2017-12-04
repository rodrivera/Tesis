#!/bin/bash
AUX=""
#AUX="../"

iFile="$1"
intervals="$AUX""data/intervals/""$iFile""/100000.txt"

if [ ! -f "$intervals" ]; then
    echo "Intervals file not found!"
    echo "Usage: testAll.sh intervals_TYPE"
    exit
fi

declare -a SUFFIXES=("0" "1" "10" "25" "50")
for N in "${SUFFIXES[@]}"
do
	queries="$AUX""data/queries/""$N"".txt"

	outR="$AUX""data/output/R/""$N""-""$iFile"".txt"
	outI="$AUX""data/output/I/""$N""-""$iFile"".txt"
	outS="$AUX""data/output/S/""$N""-""$iFile"".txt"
	outC1="$AUX""data/output/C1/""$N""-""$iFile"".txt"

	echo "----- Intervals number = 100000 ""$iFile"" and QueryWindow size = ""$N"".. -----"
	#echo "Executing.. FNR-tree with ""$N"" objects.."
	./"$AUX"rtest.out $intervals $queries $outR
	#echo "Executing..        X with ""$N"" objects.."
	./"$AUX"itest.out $intervals $queries $outI
	#echo "Executing..  Schmidt with ""$N"" objects.."
	./"$AUX"stest.out $intervals $queries $outS
	#echo "Executing..  Compressed I with ""$N"" objects.."
	./"$AUX"c1test.out $intervals $queries $outC1

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