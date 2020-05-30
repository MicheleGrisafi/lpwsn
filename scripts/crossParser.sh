#!/usr/local/bin/bash
if [ $# -lt 1 ]; then
    printf "Syntax Error: $0 fast[0|1] [folder_pattern] [discAvgWeight] [discDevWeight] [dutyWeight] [prefNode] [prefNodeWeight]\n"
    exit 1
fi
fast=$1
pattern=$2
avgW=$3
devW=$4
dutW=$5
prefNode=$6
prefNodeW=$7

if [ $pattern == "" ]; then
    pattern="*"
fi

echo $pattern

declare -A results


echo "" > results.txt
for d in logs/${pattern}/; do
    set=$(basename $d)
    echo "LogSet: $set"
    python3 parser.py $set $fast $avgW $devW $dutW $prefNode $prefNodeW >> results.txt
    #results[$set]=$(python3 parser.py $set $fast $avgW $devW $dutW $prefNode $prefNodeW | grep 'Avg Perf' | cut -d " " -f 3)
    results[$set]=$(cat results.txt | grep 'Avg Perf' | tail -1 | cut -d " " -f 3)
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
printf "\n\033[95m\033[91mMax is $maxK with an avg performance of ${results[$maxK]}\033[0m\n\n" >> results.txt