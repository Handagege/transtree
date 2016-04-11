#!/bin/sh

cd ../../bin
./lab_common_svr ../conf/wooTest.conf > ../log/wooTestReq.log 2>&1 &
cd -
