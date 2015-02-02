#!/bin/bash

RESULTS="./brkga_results.txt"

echo "" > $RESULTS
for i in 1 5 10 50 100 1000
do
	echo "BRKGA $i" >> "./brkga_results.txt"
	printf " " >> "./brkga_results.txt"
	OMP_NUM_THREADS=8 /usr/bin/time -f %e -a -o $RESULTS ./BRKGA_$i < ../Metaheuristic/Tests_final/InputC++_small1

	printf " " >> "./brkga_results.txt"
	OMP_NUM_THREADS=8 /usr/bin/time -f %e -a -o $RESULTS ./BRKGA_$i < ../Metaheuristic/Tests_final/InputC++_small2

	printf " " >> "./brkga_results.txt"
	OMP_NUM_THREADS=8 /usr/bin/time -f %e -a -o $RESULTS ./BRKGA_$i < ../Metaheuristic/Tests_final/InputC++_small3

	printf " " >> "./brkga_results.txt"
	OMP_NUM_THREADS=8 /usr/bin/time -f %e -a -o $RESULTS ./BRKGA_$i < ../Metaheuristic/Tests_final/InputC++_normal

	printf " " >> "./brkga_results.txt"
	OMP_NUM_THREADS=8 /usr/bin/time -f %e -a -o $RESULTS ./BRKGA_$i < ../Metaheuristic/Tests_final/InputC++_normal2

	printf " " >> "./brkga_results.txt"
	OMP_NUM_THREADS=8 /usr/bin/time -f %e -a -o $RESULTS ./BRKGA_$i < ../Metaheuristic/Tests_final/InputC++_large

	printf "\n" >> "./brkga_results.txt"
done

#-------------------------------------------------------------------#
#-------------------------------------------------------------------#

