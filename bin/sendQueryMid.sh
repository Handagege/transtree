#!/bin/sh

if [ $# -ne 2 ]
then
        echo "case <rmid> <kmid>"
        exit -1
fi


#reqStr="{\"api\":\"erlangshen\",\"cmd\":\"insertqn\",\"body\":{\"rmid\":\"$1\",\"kmid\":\"$2\"}}"
reqStr="{\"api\":\"erlangshen\",\"cmd\":\"fn_insert\",\"body\":{\"rmid\":\"$1\",\"pmid\":\"$2\",\"kmid\":\"1234567891011\"}}"
#reqStr="{\"api\":\"erlangshen\",\"cmd\":\"deleteqn\",\"body\":{\"rmid\":\"$1\",\"kmid\":\"$2\"}}"
python2.6 binserverclient.py 127.0.0.1 10022 $reqStr
