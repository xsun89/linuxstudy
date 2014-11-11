#!/bin/bash

part="/dev/disk1"
look_out=`df | grep "$part" | awk '{print $5}'|sed 's/%//g'`
echo $look_out
until [ $look_out -gt "25" ] 
do
	echo -e "file system $part is nearly full" 
	look_out=`df |grep "part"|awk '{print $5}' |sed 's/%//g'`
	sleep 3
done

