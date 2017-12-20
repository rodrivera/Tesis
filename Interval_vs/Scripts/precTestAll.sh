#!/bin/bash
AUX=""
#AUX="../"

iDir="prec800k"

declare -a SUFFIXES=("0" "2" "4" "6" "8")
for N in "${SUFFIXES[@]}"
do
	intervals="$AUX""data/intervals/""$iDir""/""$N"".txt"
	queries="$AUX""data/queries/precision/""$N"".txt"
	outR="$AUX""data/output/R/prec_""$iDir""-""$N"".txt"
	outI="$AUX""data/output/I/prec_""$iDir""-""$N"".txt"
	outS="$AUX""data/output/S/prec_""$iDir""-""$N"".txt"
	outC1="$AUX""data/output/C1/prec_""$iDir""-""$N"".txt"
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
			:
		else
			printf "¡¡ Failed (Schmidt) !!\n"
			sdiff $outR $outS
			exit
		fi
		if diff $outR $outC1 >/dev/null; then
			:
		else
			printf "¡¡ Failed (C1) !!\n"
			sdiff $outR $outC1
			exit
		fi
		printf "Passed\n"
	else
		printf "¡¡ Failed !!\n"
		sdiff $outR $outI
		exit
	fi
	echo "-------------------------------------------"; echo ""
done
