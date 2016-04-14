#!/bin/sh

if [ $# -ne 2 ]
then
        echo "case <rmid> <kmid>"
        exit -1
fi


reqStr="{\"api\":\"erlangshen\",\"cmd\":\"insertqn\",\"body\":{\"rmid\":\"$1\",\"kmid\":\"$2\"}}"
python2.6 binserverclient.py 127.0.0.1 10022 $reqStr
