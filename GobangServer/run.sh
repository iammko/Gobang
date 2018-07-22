#!/bin/bash


SERVER=GobangServer

ulimit -c unlimited
ulimit -m 1024 * 1024

function start()
{
	echo start server
	./$SERVER &
	sleep 0.5
}

function stop()
{
	echo stop server
	ps x | grep $SERVER | grep -v grep | awk '{print $1}' | xargs kill -9 
	sleep 0.5
}

for cmd in $@
do
	if [ $cmd == "start" ]
	then
		stop
		start
	elif [ $cmd == "stop" ]
	then
		stop
	fi
done
