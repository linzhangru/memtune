#! /bin/sh

rm -f log

for q in $(seq 1 100)
do
    echo $q
    ./mmc >> log
    echo >> log
done
