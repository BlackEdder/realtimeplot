#!/bin/bash
# Immediately run once
ctest

while : 
do
	file=`inotifywait -q -e CREATE bin/ --format %f`
	if [[ "$file" =~ unittest_* ]]
	then
		sleep 2
		ctest
	fi
done
