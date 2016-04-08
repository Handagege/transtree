#!/usr/bin/python2.6
# -*- coding: utf-8 -*-
import time
import sys
import urllib2
import random
import socket
import struct
from time import sleep
try:
    import simplejson as json
except Exception,data:
    import json

import binserverclient

'''
从firehose中读取数据
http://wiki.intra.weibo.com/Firehose%E6%8E%A5%E5%85%A5%E8%AF%B4%E6%98%8E
http://wiki.intra.weibo.com/Firehose_2.0%E6%8E%A5%E5%85%A5%E8%AF%B4%E6%98%8E

1、微博（）
type  = "status"
event = ["add"(发博，包括原创和转发), "update", "delete"]
"http://firehose0.i.api.weibo.com:8082/comet?appid=interestfeed"

'''
FIREHOSE_ID_LEN = 16

class Firehose(object):
    def __init__(self, baseurl):
        self.type = 'firehose'
        self.sinceId = "false"
        self.baseurl = baseurl
        self.open_firehose()
        
    def open_firehose(self):
        self.url = self.baseurl + self.sinceId
        req = urllib2.Request(self.url)
        self.conn = urllib2.urlopen( req )

    def parse_data(self):
        pass

    def read_data(self):
        st = time.time()
        # 如果连接发生错误，重连
        if not self.conn:
            self.open_firehose()

        # 如果返回码不是200,这是连接被firehose踢了，重连
        # 被踢了conn_stat_code也是200,这是什么机制??
        self.firehose_data = self.conn.readline()
        conn_stat_code = self.conn.getcode()
        if conn_stat_code != 200:
            self.open_firehose()
            self.firehose_data = self.conn.readline()
            
        # 有时候是空的数据,这是连接被firehose踢了，重连
        if not self.firehose_data:
            self.open_firehose()
            #self.firehose_data = self.conn.readline().strip()       # tail with \r\n ?
            self.firehose_data = self.conn.readline() 

        # 重连时需要记录sinceId
        try:
            self.firehose_dict = json.loads(self.firehose_data)
            self.sinceId = str(self.firehose_dict["id"])
        except Exception,data:
            print data


    def read_line(self):
        self.result_dict = {}
        self.read_data()
        return self.parse_data()


# 微博行为 
class WeiboFirehose(Firehose):
    def __init__(self):
        self.type = 'weibo'
        self.sinceId = "false"
        # 为了防止firehose真的读到了空的数据
        self.baseurl = "http://firehose0.i.api.weibo.com:8082/comet?appid=interestfeed&filter=status,*&loc="
        self.open_firehose()

    def parse_data(self):
        try:
            result_dict = {}
            weibo_content = self.firehose_dict['text']['status']
            event = self.firehose_dict['text']['event']
            if event == "add":
                result_dict['kmid'] = weibo_content['mid']
                #result_dict['uid'] = weibo_content['user']['id']
                if 'parent_rt_id_db' in weibo_content:
                        result_dict['type'] = 'transmit'
                        result_dict['pmid'] = str(weibo_content['parent_rt_id_db'])
                        result_dict['rmid'] = weibo_content['retweeted_status']['mid']
                else:
                        result_dict['type'] = 'original'
                        result_dict['rmid'] = result_dict['kmid']
        except Exception, e:
            print e
        return result_dict


#监听器
class Listener():
        def __init__(self,host,listenPort):
                try:
                        self.listenSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                        self.listenSock.bind((host, listenPort))
                        self.listenSock.listen(1)
                except:
                        print 'init socket err!'
                        exit(0)
                self.queryNodeSet = set()
                self.conn,self.addr = self.listenSock.accept()
                self.conn.settimeout(0.5)
                print self.conn,self.addr

        def answer(self):
                try:
                        cmdJson = self.conn.recv(1024)
                        print cmdJson
                        if cmdJson:
                                cmdDict = json.loads(cmdJson)
                                self.conn.sendall(self.deal(cmdDict))
                                print 'query node number : {0}'.format(len(self.queryNodeSet))
                        else:
                                return False
                except socket.timeout:
                        #pass
                        print 'time out'
                return True

        
        def labCommonAnswer(self):
                try:
                        reqHead = self.conn.recv(16)
                        if not reqHead:
                                print 'None data'
                                return False
                        body_len, log_id, tmp1, tmp2 = struct.unpack('IIII', reqHead)
                        if body_len == 0:
                                reqData = 'None data'
                        else :
                                recved_data = []
                                recved_len = 0 
                                while recved_len < body_len:
                                        data = self.conn.recv(body_len - recved_len)
                                        if len(data) <= 0:
                                                return False
                                        recved_data.append(data)
                                        recved_len += len(data)
                                reqData = ''.join(recved_data)
                        
                        print reqData
                        cmdDict = json.loads(reqData)
                        respStr = self.deal(cmdDict)
                        head = struct.pack('IIII', len(respStr), log_id, 0, 0)
                        ret = self.conn.send(head)
                        if ret != len(head) :
                                return False
                        self.conn.sendall(respStr)
                        print respStr
                except socket.timeout:
                        print 'time out'
                except Exception, e:
                        print e
                        return False
                return True


        def deal(self,cmdDict):
                mids = set(cmdDict['mids'])
                if cmdDict['cmd'] == 'add':
                        self.queryNodeSet.update(mids)
                        return 'add succ'
                if cmdDict['cmd'] == 'del':
                        self.queryNodeSet -= mids
                        return 'del succ'
                else:
                        print "command not found ..."
                        return 'check cmd string'

        def reConnect(self):
                self.conn.close()
                self.conn,self.addr = self.listenSock.accept()
                print self.addr
                self.conn.settimeout(0.5)



rmidKey = 'rmid'
pmidKey = 'pmid'
kmidKey = 'kmid'
cmdKey = 'cmd'
weiboType = 'type'
ISOTIMEFORMAT='%Y-%m-%d %X'


def convertWeribo(minute,host,port,listenHost,listenPort=10021):
        f_weibo= WeiboFirehose()
        lis = Listener(listenHost,listenPort)
        tbeg = int(time.time())
        duration = 60 * minute
        queryDict = {}
        queryDict['api'] = 'mtree'
        upperQueryNodeNum = 50
        insertFlag = False
        cmdCount = 1

        while True:
                answerFlag = lis.labCommonAnswer()
                print answerFlag
                if not answerFlag:
                        lis.reConnect()

                #读取firehose微博数据
                #weibo_route_data = f_weibo.read_line()
                #if weibo_route_data:
                #        #构造查询字串
                #        queryDict['body'] = weibo_route_data
                #        if weibo_route_data[weiboType] == 'original':
                #                if len(lis.queryNodeSet) < upperQueryNodeNum:
                #                        lis.queryNodeSet.add(weibo_route_data[kmidKey])
                #                        queryDict[cmdKey] = 'insertqn'
                #                        insertFlag = True
                #        elif weibo_route_data[weiboType] == 'transmit' and \
                #                weibo_route_data[rmidKey] in lis.queryNodeSet:
                #                        queryDict[cmdKey] = 'insertfn'
                #                        insertFlag = True
                #        if insertFlag: 
                #                reqStr = json.dumps(queryDict)
                #                respStr = binserverclient.reqbinserver_cluster([{"host":host,"port":port}],0,reqStr)
                #                printCMDinfo(cmdCount,reqStr,respStr)
                #                cmdCount += 1
                #        insertFlag = False
                #else:
                #        pass
                #        #print "None data"
                if time.time() - tbeg >= duration:
                        break
        lis.conn.close()



def printCMDinfo(cmdCount,reqStr,respStr):
        print "\n*********[cmd number : {0}]*********".format(cmdCount)
        print "requst : {0}".format(reqStr)
        print "response : {0}".format(respStr)
        print "at time : -- {0} --".format(time.strftime(ISOTIMEFORMAT, time.localtime()))


def convertLocal():
        inputFile = sys.argv[1]
        host = sys.argv[2]
        port = int(sys.argv[3])
        f = open(inputFile,'r')
        data = json.load(f)
        for i in data:
                time.sleep(1)
                #lcsreq.request(host,port,json.dumps(i))


if __name__ == '__main__':
        if len(sys.argv) != 5:
                print "case arguments : <runTimePeriod> <requestHost> <requestPort> <listenPort>"
                exit(0)
        minute   = float(sys.argv[1])
        host = sys.argv[2]
        port = int(sys.argv[3])
        listenPort = int(sys.argv[4])
        convertWeribo(minute,host,port,'10.77.96.32',listenPort)

