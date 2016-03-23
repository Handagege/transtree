#!/usr/bin/python2.6
# -*- coding: utf-8 -*-
import time
import sys
import traceback
import urllib2
import random
from time import sleep
from uuid import uuid4
try:
    import simplejson as json
except Exception,data:
    import json

import lcsreq


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
        # 为了防止firehose真的读到了空的数据
        self.empty_set = set([])
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
                result_dict['uid'] = weibo_content['user']['id']
                if 'parent_rt_id_db' in weibo_content:
                        result_dict['type'] = 'transmit'
                        result_dict['pmid'] = weibo_content['parent_rt_id_db']
                        result_dict['rmid'] = weibo_content['retweeted_status']['mid']
                else:
                        result_dict['type'] = 'original'
                        result_dict['rmid'] = result_dict['kmid']
        except Exception, e:
            print e
        return result_dict


def convertWeribo():
        minute   = float(sys.argv[1])
        f_weibo= WeiboFirehose()
        tbeg = int(time.time())
        duration = 60 * minute
        queryDict = {}
        queryDict['api'] = 'mtree'
        upperQueryNodeNum = 100
        queryNodeSet = set()
        rmidKey = 'rmid'
        pmidKey = 'pmid'
        kmidKey = 'kmid'
        cmdKey = 'cmd'
        weiboType = 'type'
        while True:
                weibo_route_data = f_weibo.read_line()
                if weibo_route_data:
                        queryDict['body'] = weibo_route_data
                        if weibo_route_data[weiboType] == 'original':
                                if len(queryNodeSet) < upperQueryNodeNum:
                                        queryNodeSet.add(weibo_route_data[kmidKey])
                                        queryDict[cmdKey] = 'insertqn'
                                        print f_weibo.sinceId,json.dumps(queryDict)
                        elif weibo_route_data[weiboType] == 'transmit' and \
                                weibo_route_data[rmidKey] in queryNodeSet:
                                        queryDict[cmdKey] = 'insertfn'
                                        print f_weibo.sinceId,json.dumps(queryDict)
                        queryDict.clear()
                if time.time() - tbeg >= duration:
                        break

def convertLocal():
        inputFile = sys.argv[1]
        f = open(inputFile,'r')
        data = json.load(f)
        for i in data:
                time.sleep(1)
                lcsreq.request()
        


if __name__ == '__main__':
        convertLocal()

