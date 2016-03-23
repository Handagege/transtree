#!/usr/bin/python


import struct
import socket
import sys


def request(host, port, requestStr, log_id = 0, timeout = 1):
        address = (host,port)
        print address,requestStr
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
                s.settimeout(timeout)
                s.connect(address)
                head = struct.pack('IIII',len(requestStr),log_id,0,0)
                ret = s.send(head)
                if ret != len(head):
                        s.close()
                        return False
                ret = s.send(requestStr)
                head = s.recv(16)
                body_len, log_id, tmp1, tmp2 = struct.unpack('IIII', head)
                print body_len
                data = s.recv(body_len)
                print data
        except Exception, e:
                pass
        s.close()
        return False


if __name__=='__main__':
        host = sys.argv[1]
        port = int(sys.argv[2])
        requestStr = sys.argv[3]
        request(host,port,requestStr)

