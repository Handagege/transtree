#!/bin/sh

cd ../../bin
./lab_common_svr ../conf/mtree.conf > ../log/mtreeReq.log 2>&1 &
#./lab_common_svr ../conf/wooTest.conf > ../log/wooTestReq.log 2>&1 &
cd -
