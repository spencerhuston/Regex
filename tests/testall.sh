#!/bin/bash

(cd .. && make clean)
(cd .. && make)
(cd .. && rm *.gch)
clear

function runtest {

	empty=''
	if [ $# == 8 ]; then
		empty=$8
	fi
	../regex.exe $2 $3 $4 $5 $6 $7 "$empty"> r$1.out
	diff r$1.out r$1.org > diff.out
	if [ $? -eq 0 ]; then
		echo Test $1 passed...;
	else
		echo Test $1 Failed
		echo ------ Your Output ----------
		cat r$1.out
		echo ------ Expected Output ------
		cat r$1.org
		echo ------ Difference -----------
		cat diff.out
		echo -----------------------------
	fi

	echo 
}

function printResults {

	echo Individual character tests
	echo --------------------------	
	# concatenation
	runtest '1' 'ab' 'a' 'b' 'ab' 'ba' 'c'
	
	# *
	runtest '2' 'a*c*' 'a' 'c' 'ac' 'ca' 'aacc'
	
	# +
	runtest '3' 'a+c+' 'a' 'c' 'ac' 'ca' 'aacc'

	# ?
	runtest '4' 'a?c?' 'a' 'c' 'ac' 'ca' 'aacc'

	# \
	runtest '5' '\??\**\++' '+' '?+' '?*+' '??+' '**++' '?**++'

	# |
	runtest '6' 'a|b|c' 'a' 'b' 'c' 'ab' 'ad'

	# [ - ]
	runtest '7' '[a-c]' 'a' 'b' 'c' 'ab' 'd'
}

printResults
