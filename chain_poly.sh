#!/bin/bash

if [ $# -eq 2 ] && [ -x "$1" ] && [ -d "$2" ]; then
    prog=$1
    directory=$2
    
    for file in "$directory"/*; do
        line=$(head -n 1 "$file")
        if [[ "$line" == "START" ]]; then
            next_file="$file"
        fi
    done
    
    last_line=$(tail -n 1 "$next_file")
    sed -e '1d' -e '$d' "$next_file" > test.in
    
    while [[ "$last_line" != "STOP" ]]; do
        ./"$prog" < test.in > output
        cat output > test.in
        next_file=${last_line:5}
        last_line=$(tail -n 1 "$directory"/"$next_file")
        sed -e '$d' "$directory"/"$next_file" >> test.in
    done
    
    ./"$prog" < test.in
    
    rm output
    rm test.in
    exit 0
    
elif [ ! $# -eq 2 ]; then
    echo "Wrong number of parameters"
    echo "Correct format is ./chain_poly.sh (program) (directory of tests)"
    exit 1
elif [ ! -x "$1" ]; then
    echo "First argument isn't an executable file"
    echo "Correct format is ./chain_poly.sh (program) (directory of tests)"
    exit 1
elif [ ! -d "$2" ]; then
    echo "Second argument isn't a directory"
    echo "Correct format is ./chain_poly.sh (program) (directory of tests)"
    exit 1
fi