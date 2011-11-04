#!/bin/sh
# Immediately run once
ctest

while : 
do
	file=`inotifywait -q -e CREATE bin/ --format %f`
	if [[ "$file" =~ unittest_* ]]
	then
		ctest
	fi
done
