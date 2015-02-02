#!/bin/bash



RESULTS_TIME="./brkga_results_time.txt"
RESULTS_F="./brkga_results_F.txt"

for i in 10 20 30 40 50 60 70 80 90 100
do
	for j in 1 2 3 4 5
	do
		for k in 100 200 300 400 500 600 700 800 900 1000
		do
			NAME="Test_$i""_$j""_$k""C++"
			for ii in 1 5 10 50 100 1000
			do
				OMP_NUM_THREADS=8 /usr/bin/time -f %e -a -o $RESULTS_TIME"_$ii" ./BRKGA_$ii < ../Metaheuristic/Tests_final/$NAME >> $RESULTS_F"_$ii"
			done
		done
	done
done
