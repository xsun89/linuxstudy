#!/bin/bash

test -w 1.txt
echo $?
[ -w 2.txt ]
if [ $? -eq 0 ]; then
	echo "writable";
else
	echo "not writable";
fi

