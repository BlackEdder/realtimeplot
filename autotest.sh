#!/bin/sh
while inotifywait bin/unittest_* -q; do
	sleep 1
	clear
	ctest
done
