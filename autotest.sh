#!/bin/sh
while inotifywait bin/unittest_* -q; do
	clear
	ctest
done
