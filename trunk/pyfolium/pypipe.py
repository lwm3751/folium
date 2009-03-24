#coding=utf-8
import sys
import os
import datetime

import pipe

class Pipe(object):
    def __init__(self):
        self.io = pipe.stdio
        self.logfile = None
        logdir = os.path.join(os.path.dirname(sys.argv[0]), 'log')
        if not os.path.exists(logdir):
            os.mkdir(logdir)
        for i in range(0, 256):
            filename = os.path.join(logdir, '%s_%d.txt'%(datetime.date.today(), i))
            if not os.path.exists(filename):
                self.logfile = open(filename, 'w')
                break
    def __del__(self):
        if self.logfile:
            self.logfile.flush()
    def readable(self):
        return self.io.readable()
    def readline(self):
        line = self.io.readline()
        self.logfile.write("read line:%s\n" % line)
        return line
    def writeline(self, line):
        self.logfile.write("write line:%s\n" % line)
        self.io.writeline(line)
