#!/bin/sh


./converter.py 127.0.0.1 10022 10.77.96.32 10021 > converter.log 2>&1 &

#ps -u | grep ./converter.py | grep -v grep | awk {'print $2'}  | xargs kill -9
#ps -u | grep ./lab_co | grep -v grep | awk {'print $2'}  | xargs kill -9
