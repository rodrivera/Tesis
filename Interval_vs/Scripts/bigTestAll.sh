#!/bin/bash
AUX=""
#AUX="../"

prefix="$1"
qFile="$2"
intervals_dir="$AUX""data/intervals/""$prefix""/"
queries="$AUX""data/queries/""$qFile"".txt"

if [ ! -d "$intervals_dir" ]; then
	echo "Intervals directory not found!"
	echo "Usage: testAll.sh intervals_cat search_window_SIZE"
    echo "with size between 0 and 100 or rrr"
    exit
fi

if [ ! -f "$queries" ]; then
    echo "Queries file not found!"
    echo "Usage: testAll.sh intervals_cat search_window_SIZE"
    echo "with size between 0 and 100 or rrr"
    exit
fi

declare -a SUFFIXES=("100000" "200000" "400000" "800000")
for N in "${SUFFIXES[@]}"
do
	intervals="$intervals_dir""$N"".txt"
	outR="$AUX""data/output/R/""$qFile""-""$prefix""$N"".txt"
	outI="$AUX""data/output/I/""$qFile""-""$prefix""$N"".txt"
	outS="$AUX""data/output/S/""$qFile""-""$prefix""$N"".txt"
	outC1="$AUX""data/output/C1/""$qFile""-""$prefix""$N"".txt"
	echo "----- Intervals size = ""$N"" and QueryWindow size = ""$qFile"".. -----"
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
