#!/bin/bash



if [ $# -lt 2 ]; then
    printf "Syntax Error: $0 folder fast[0|1]\n"
    exit 1
fi
folder=$1
fast=$2

if [ -d "$folder" ]; then
    # Take action if $DIR exists. #
    echo "Simulating ${folder}"
    simulations="${folder}/simulations/*.csc"
else
    printf "Folder does not exist\n"
    exit 1
fi

if [ $fast -eq 1 ]; then
    simulations="${folder}/simulations/*-1.csc"
fi


for filename in $simulations; do
    printf "Testing ${filename}\n"
    cooja_nogui $filename > /dev/null
    name=$(basename $folder)
    sim=$(basename $filename)
    mkdir -p logs/${name}
    printf "Moving logs to logs/${name}/${sim}.dc.log\n"
    mv test_dc.log "logs/${name}/${sim}.dc.log"
    mv test.log "logs/${name}/${sim}.log"
done
rm *log

echo "Done simulating ${folder}"
