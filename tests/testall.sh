#!/bin/bash

(cd .. && make clean)
(cd .. && make)
clear

function runtest {
	args=$1
	
	../regex.exe $args > r$2.out
	diff r$2.out r$2.org > diff.out
	if [ $? -eq 0 ]; then
		echo Test passed...;
	else
		echo Test Failed
		echo ------ Your Output ----------
		cat r$2.out
		echo ------ Expected Output ------
		cat r$2.org
		echo ------ Difference -----------
		cat diff.out
		echo -----------------------------
	fi
}

function printResults {
	
	echo Scanner
	runtest 'ab' '1'
	runtest 'a(a|b)*' '2'
	runtest 'a[a-c]+' '3'
}

printResults
