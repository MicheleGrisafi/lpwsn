#!/bin/bash



if [ $# -lt 2 ]; then
    printf "Syntax Error: $0 folder fast[0|1]\n"
    exit 1
fi
folder=$1
fast=$2
root=$(pwd)

for d in ${folder}*/; do
    echo "Simulation: $d"
    cd $d && make > /dev/null
    cd $root
    ./simulate.sh $d $fast
done




