#! /bin/sh

rm -f log

for q in $(seq 1 10)
do
    echo $q
    ./mmc >> log
    echo >> log
done
