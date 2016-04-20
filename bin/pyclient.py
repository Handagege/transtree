#!/usr/bin/python

import socket
import json
import random
import time
import binserverclient
import sys


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
	        reqDict = {'cmd':cmdList[random.randint(0,1)]}
		reqDict['mids'] = random.sample(mids,random.randint(1,10))
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
	        reqDict = {'cmd':cmdList[random.randint(0,1)],'api':'wooTest'}
		reqDict['rmids'] = random.sample(mids,random.randint(1,10))
		reqJson = json.dumps(reqDict)
                respStr = binserverclient.reqbinserver_cluster([{'host':'10.77.96.32','port':10022}],0,reqJson)
                printCMDinfo(count,reqJson,respStr)
                count += 1


def sendMonitoringMid():
        #if len(sys.argv) != 4:
        #        print 'case <cmd> <rmid> <kmid>'
        #        exit(0)
        reqDict = {'cmd':'qn_insert','api':'erlangshen','body':{'rmid':sys.argv[2],'kmid':sys.argv[3]}}
        #reqDict = {'cmd':'xx','api':'mtree','rmids':['1234567890']}
        #respStr = binserverclient.reqbinserver_cluster([{'host':'10.77.96.94','port':60138}],0,reqJson)
        reqJson = json.dumps(reqDict)
        respStr = binserverclient.reqbinserver_cluster([{'host':'10.77.96.32','port':10022}],0,reqJson)
        printCMDinfo(1,reqJson,respStr)


def recvMonitoringMid():
        reqDict = {'cmd':'analysis','api':'erlangshen','body':'xxxx'}
        reqJson = json.dumps(reqDict)
        respStr = binserverclient.reqbinserver_cluster([{'host':'10.77.96.32','port':10022}],0,reqJson)
        printCMDinfo(1,reqJson,respStr)


def randomSend(sendNum=300):
        rmids  = []
        for i in range(0,sendNum):
                rmid = str(random.randint(0,900000))
                rmids.append(rmid)
                reqDict = {'cmd':'qn_insert','api':'erlangshen','body':{'rmid':rmid,'kmid':rmid}}
                reqJson = json.dumps(reqDict)
                respStr = binserverclient.reqbinserver_cluster([{'host':'10.77.96.32','port':10022}],0,reqJson)
                printCMDinfo(1,reqJson,respStr)
                for j in range(0,499):
                        time.sleep(0.0008)
                        reqDict = {'cmd':'fn_insert','api':'erlangshen','body':{'rmid':rmid,
                                'kmid':str(random.randint(0,100000000000000)),'pmid':rmid}}
                        reqJson = json.dumps(reqDict)
                        respStr = binserverclient.reqbinserver_cluster([{'host':'10.77.96.32','port':10022}],0,reqJson)
        print "add 499 finish"
        time.sleep(10)
        for i in rmids:
                reqDict = {'cmd':'deleteqn','api':'erlangshen','body':{'rmid':i,
                        'kmid':i}}
                reqJson = json.dumps(reqDict)
                respStr = binserverclient.reqbinserver_cluster([{'host':'10.77.96.32','port':10022}],0,reqJson)


def qingguoSend():
        rmid = "3963813327829999"
        reqDict = {'cmd':'qn_insert','api':'erlangshen','body':{'rmid':rmid,'kmid':rmid}}
        reqJson = json.dumps(reqDict)
        respStr = binserverclient.reqbinserver_cluster([{'host':'10.77.96.94','port':60128}],0,reqJson)
        printCMDinfo(1,reqJson,respStr)
        one_kmid = ""
        two_kmid = ""
        three_kmid = ""
        four_kmid = ""
        for i in range(0,499):
                time.sleep(0.005)
                one_kmid = "396381332782"+str(i)
                reqDict = {'cmd':'fn_insert','api':'erlangshen','body':{'rmid':rmid,
                        'kmid':one_kmid,'pmid':rmid}}
                reqJson = json.dumps(reqDict)
                respStr = binserverclient.reqbinserver_cluster([{'host':'10.77.96.94','port':60128}],0,reqJson)
                printCMDinfo(1,reqJson,respStr)
        for i in range(499,998):
                time.sleep(0.005)
                two_kmid = "396381332782"+str(i)
                reqDict = {'cmd':'fn_insert','api':'erlangshen','body':{'rmid':rmid,
                        'kmid':two_kmid,'pmid':one_kmid}}
                reqJson = json.dumps(reqDict)
                respStr = binserverclient.reqbinserver_cluster([{'host':'10.77.96.94','port':60128}],0,reqJson)
                printCMDinfo(1,reqJson,respStr)
        for i in range(998,1497):
                time.sleep(0.005)
                three_kmid = "396381332782"+str(i)
                reqDict = {'cmd':'fn_insert','api':'erlangshen','body':{'rmid':rmid,
                        'kmid':three_kmid,'pmid':two_kmid}}
                reqJson = json.dumps(reqDict)
                respStr = binserverclient.reqbinserver_cluster([{'host':'10.77.96.94','port':60128}],0,reqJson)
                printCMDinfo(1,reqJson,respStr)
        for i in range(1497,1996):
                time.sleep(0.005)
                four_kmid = "396381332782"+str(i)
                reqDict = {'cmd':'fn_insert','api':'erlangshen','body':{'rmid':rmid,
                        'kmid':four_kmid,'pmid':three_kmid}}
                reqJson = json.dumps(reqDict)
                respStr = binserverclient.reqbinserver_cluster([{'host':'10.77.96.94','port':60128}],0,reqJson)
                

                
def errorInput():
        print 'Can\'t find this command...'


ISOTIMEFORMAT = '%Y-%m-%d %X'

def printCMDinfo(cmdCount,reqStr,respStr):
        print '\n*********[cmd number : {0}]*********'.format(cmdCount)
        print 'requst : {0}'.format(reqStr)
        print 'response : {0}'.format(respStr)
        print 'at time : -- {0} --'.format(time.strftime(ISOTIMEFORMAT, time.localtime()))


if __name__=='__main__':
        cmd = sys.argv[1]
        cmdDict = {}
        cmdDict['send'] = sendMonitoringMid
        cmdDict['recv'] = recvMonitoringMid
        cmdDict['rsend'] = randomSend
        cmdDict['qsend'] = qingguoSend
        f = cmdDict.get(cmd,errorInput)
        f()


