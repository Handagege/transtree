#!/bin/sh

cd ../../bin
./lab_common_svr ../conf/wooTest.conf > ../log/wooTestReq.log 2>&1 &
cd -

./converter.py 1 12 23 10021 > runFilterAnswer.log 2>&1 &

./pyclient.py > runFilterCMD.log 2>&1 &

