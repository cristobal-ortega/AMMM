#!/bin/bash

PATH="./"
EXE="./ammm_testgenerator_auto.py"
DEBUG="./list"


echo "" > $DEBUG
for i in 10 20 30 40 50 60 70 80 90 100
do
	for j in 2 3 4 5
	do
		for k in 100 200 300 400 500 600 700 800 900 1000
		do
			NAME="Test_$i""_$j""_$k"
			echo $NAME >> $DEBUG
			/usr/bin/python $EXE $PATH$NAME $i $j $k
			/bin/sleep 30
		done
	done
done
