#!/bin/bash
if [ $# -lt 1 ]; then
    printf "Syntax Error: $0 fast[0|1] [discAvgWeight] [discDevWeight] [dutyWeight] [prefNode] [prefNodeWeight]\n"
    exit 1
fi
fast=$1
avgW=$2
devW=$3
dutW=$4
prefNode=$5
prefNodeW=$6

declare -A results=()

for d in logs/*/; do
    set=$(basename $d)
    #echo "LogSet: $set"
    python parser.py $set $fast $avgW $devW $dutW $prefNode $prefNodeW
    results[$set]=$(python parser.py $set $fast $avgW $devW $dutW $prefNode $prefNodeW | grep 'Avg Perf' | cut -d " " -f 3)
done

max=0
maxK=0

for K in "${!results[@]}"; do 
    if [ "$(echo "${results[$K]} > ${max}" | bc)" -eq 1 ]; then
        max=${results[$K]}
        maxK=$K
    fi
done
printf "\n\033[95m\033[91mMax is $maxK with an avg performance of ${results[$maxK]}\033[0m\n\n"