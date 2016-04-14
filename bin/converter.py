#!/usr/bin/python2.6
# -*- coding: utf-8 -*-
import time
import sys
import urllib2
import random
import socket
import struct
import threading
from time import sleep
try:
    import simplejson as json
except Exception,data:
    import json

import binserverclient


rmidKey = 'rmid'
pmidKey = 'pmid'
kmidKey = 'kmid'
cmdKey = 'cmd'
weiboType = 'type'
ISOTIMEFORMAT='%Y-%m-%d %X'


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
    def __init__(self,host,port):
        self.type = 'weibo'
        self.sinceId = "false"
        self.sendHost = host
        self.sendPort = port
        # 为了防止firehose真的读到了空的数据
        self.baseurl = "http://firehose0.i.api.weibo.com:8082/comet?appid=interestfeed&filter=status,*&loc="
        self.open_firehose()

    def parse_data(self):
        try:
            result_dict = {}
            weibo_content = self.firehose_dict['text']['status']
            event = self.firehose_dict['text']['event']
            if event == "add":
                result_dict[kmidKey] = weibo_content['mid']
                #result_dict['uid'] = weibo_content['user']['id']
                if 'parent_rt_id_db' in weibo_content:
                        result_dict[weiboType] = 'transmit'
                        result_dict[pmidKey] = str(weibo_content['parent_rt_id_db'])
                        result_dict[rmidKey] = weibo_content['retweeted_status']['mid']
                else:
                        result_dict[weiboType] = 'original'
                        result_dict[rmidKey] = result_dict['kmid']
        except Exception, e:
            print e
        return result_dict


    def convert(self,ilock,queryNodeSet):
        queryDict = {}
        queryDict['api'] = 'erlangshen'
        #upperQueryNodeNum = 50
        insertFlag = False
        cmdCount = 1
        while True:
                #读取firehose微博数据
                weibo_route_data = self.read_line()
                if weibo_route_data:
                        #构造查询字串
                        queryDict['body'] = weibo_route_data
                        ilock.acquire()
                        #if weibo_route_data[weiboType] == 'original':
                        #        if len(queryNodeSet) < upperQueryNodeNum:
                        #                queryNodeSet.add(weibo_route_data[kmidKey])
                        #                queryDict[cmdKey] = 'insertqn'
                        #                insertFlag = True
                        if weibo_route_data[weiboType] == 'transmit' and \
                                weibo_route_data[rmidKey] in queryNodeSet:
                                        queryDict[cmdKey] = 'fn_insert'
                                        insertFlag = True
                        ilock.release()
                        if insertFlag: 
                                reqStr = json.dumps(queryDict)
                                respStr = binserverclient.reqbinserver_cluster( \
                                        [{"host":self.sendHost,"port":self.sendPort}],0,reqStr)
                                #print 'query node number : {0}'.format(len(queryNodeSet))
                                printCMDinfo(cmdCount,reqStr,respStr)
                                cmdCount += 1
                        insertFlag = False
                #else:
                #        print "don\'t fetch weibo firehose data this time"


def printCMDinfo(cmdCount,reqStr,respStr):
        print "\n*********[cmd number : {0}]*********".format(cmdCount)
        print "requst : {0}".format(reqStr)
        print "response : {0}".format(respStr)
        print "at time : -- {0} --".format(time.strftime(ISOTIMEFORMAT, time.localtime()))


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

        def answer(self,ilock):
                connCount = 0
                while True:
                        conn,addr = self.listenSock.accept()
                        connCount += 1
                        print '\n...create connecter {0} times...'.format(connCount)
                        conn.settimeout(50)
                        self.labCommonAnswer(conn,ilock)
                        conn.close()


        def tcpAnswer(self,conn,ilock):
                try:
                        while True:
                                cmdJson = conn.recv(1024)
                                print cmdJson
                                if cmdJson:
                                        cmdDict = json.loads(cmdJson)
                                        ilock.acquire()
                                        respStr = self.deal(cmdDict)
                                        ilock.release()
                                        conn.sendall(respStr)
                                        print 'query node number : {0}'.format(len(self.queryNodeSet))
                                else:
                                        print 'connecter {0} closed'.format((conn,addr))
                                        break
                except socket.timeout:
                        print 'recv from connecter {0} time out'.format((conn,addr))
        

        def labCommonAnswer(self,conn,ilock):
                cmdCount = 0
                try:
                        while True:
                                reqHead = conn.recv(16)
                                cmdCount += 1
                                if not reqHead:
                                        print 'request head is None'
                                        break
                                body_len, log_id, tmp1, tmp2 = struct.unpack('IIII', reqHead)
                                if body_len == 0:
                                        reqStr = 'request body is empty'
                                else :
                                        recved_data = []
                                        recved_len = 0 
                                        while recved_len < body_len:
                                                data = conn.recv(body_len - recved_len)
                                                if len(data) <= 0:
                                                        print 'receive no data'
                                                        break
                                                recved_data.append(data)
                                                recved_len += len(data)
                                                reqStr = ''.join(recved_data)
                                cmdDict = json.loads(reqStr)
                                ilock.acquire()
                                respStr = self.deal(cmdDict)
                                ilock.release()
                                head = struct.pack('IIII', len(respStr), log_id, 0, 0)
                                ret = conn.send(head)
                                if ret != len(head) :
                                        print 'can\'t sent message head'
                                        break
                                conn.sendall(respStr)
                                printCMDinfo(cmdCount,reqStr,respStr)
                                print '***query node number : {0}***'.format(len(self.queryNodeSet))
                                print self.queryNodeSet
                except socket.timeout:
                        print 'recv from connecter {0} time out'.format((conn,addr))
                except Exception, e:
                        print e


        def deal(self,cmdDict):
                mids = set(cmdDict.get('rmids',[]))
                if not mids:
                        return 'reqeust need key : \'rmids\''
                cmdStr = cmdDict.get('cmd','')
                if not cmdStr:
                        return 'reqeust need key : \'cmd\''
                if cmdStr == 'add':
                        self.queryNodeSet.update(mids)
                        return 'add succ'
                elif cmdStr == 'del':
                        self.queryNodeSet -= mids
                        return 'del succ'
                elif cmdStr == 'get':
                        return json.dumps(list(self.queryNodeSet))
                else:
                        return 'cmd not found ... heck cmd string'


def convertWeribo(host,port,listenHost,listenPort=10021):
        wf = WeiboFirehose(host,port)
        lis = Listener(listenHost,listenPort)
        ilock = threading.Lock()
        twf = threading.Thread(target=wf.convert,args=(ilock,lis.queryNodeSet))
        twf.start()
        tlis = threading.Thread(target=lis.answer,args=(ilock,))
        tlis.start()


if __name__ == '__main__':
        if len(sys.argv) != 5:
                print "case arguments : <requestHost> <requestPort> <listenHost> <listenPort>"
                exit(0)
        host = sys.argv[1]
        port = int(sys.argv[2])
        listenHost = sys.argv[3]
        listenPort = int(sys.argv[4])
        convertWeribo(host,port,listenHost,listenPort)

