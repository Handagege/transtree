#!/bin/sh


./converter.py 10.77.96.32 10022 10.77.96.32 10021 > converter.log 2>&1 &

#ps -aux | grep ./converter.py | grep -v grep | awk {'print $2'}  | xargs kill -9
