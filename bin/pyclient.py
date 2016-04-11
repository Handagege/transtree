#!/usr/bin/python

import socket
import json
import random
import time
import binserverclient


def testTcpReq():
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)    
	sock.connect(('10.77.96.32', 10021))

	mids = map(str,list(range(100000,999999)))
        cmdList = ['del','add']
        count = 0
	while True:
                if count > 5:
                        break
		time.sleep(0.1)  
	        reqDict = {"cmd":cmdList[random.randint(0,1)]}
		reqDict["mids"] = random.sample(mids,random.randint(1,10))
		reqJson = json.dumps(reqDict)
		print('send to server with value: '+ reqJson)
		sock.sendall(reqJson)    
		print(sock.recv(1024))
                count += 1
	sock.close()    


def testWooReq():
	mids = map(str,list(range(10,20)))
        cmdList = ['del','add']
        count = 0
	while True:
                if count > 9:
                        break
		time.sleep(1)  
	        reqDict = {"cmd":cmdList[random.randint(0,1)],"api":"wooTest"}
		reqDict["mids"] = random.sample(mids,random.randint(1,10))
		reqJson = json.dumps(reqDict)
                respStr = binserverclient.reqbinserver_cluster([{"host":'10.77.96.32',"port":10022}],0,reqJson)
                printCMDinfo(count,reqJson,respStr)
                count += 1

ISOTIMEFORMAT = '%Y-%m-%d %X'

def printCMDinfo(cmdCount,reqStr,respStr):
        print "\n*********[cmd number : {0}]*********".format(cmdCount)
        print "requst : {0}".format(reqStr)
        print "response : {0}".format(respStr)
        print "at time : -- {0} --".format(time.strftime(ISOTIMEFORMAT, time.localtime()))


if __name__=='__main__':
        testWooReq()
