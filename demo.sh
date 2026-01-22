#!/bin/bash
make -s
rm -f latest.txt
SECONDS=0
./build/ml data/housing.csv 0.01 10000 0.8 > latest.txt

echo "Results saved in latest.txt! (Took $SECONDS second(s))"