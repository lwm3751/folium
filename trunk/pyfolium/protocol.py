import os
import sys
import time

import folium
import pipe

import pyfolium.book.reader

class Engine(folium.Engine):
    def __init__(self):
        folium.Engine.__init__(self)
        self.book = pyfolium.book.reader.Reader()

        self.logfile = None
        filename = os.path.join(os.path.dirname(sys.argv[0]), 'snake.txt')
        self.logfile = open(filename, 'w')

    def writeline(self, line):
        folium.Engine.writeline(self, line)
        self.log("write line:%s" % `line`)
    def readline(self):
        if not self.readable():
            return
        line = folium.Engine.readline(self)
        self.log("read line:%s" % `line`)
        return line
    def log(self, s):
        if not self.logfile:
            return
        self.logfile.write("%s\n"%s)
#        self.logfile.flush()
    def __del__(self):
        self.logfile.flush()

    def run(self):
        while not self.readable():
            time.sleep(0.001)
        line = self.readline()
        if line != 'ucci':
            return

        self.writeline('id name folium')
        self.writeline('id author Wangmao Lin')
        self.writeline('id user engine tester')

        self.writeline('option usemillisec type check default true')
        self.usemillisec = True

        self.writeline('ucciok')

        self.bans = []
        self.loop()

    def loop(self):
        while True:
            while not self.readable():
                time.sleep(0.001)
            line = self.readline()
            if line == 'isready':
                self.writeline('readyok')
            elif line == 'quit' or line == 'exit':
                self.writeline('bye')
                return
            elif line.startswith('setoption '):
                self.setoption(line[10:])
            elif line.startswith('position '):
                self.position(line[9:])
            elif line.startswith('banmoves '):
                self.banmoves(line[9:])
            elif line.startswith('go'):
                self.go(line)
            elif line.startswith('probe'):
                self.probe(line)

    def position(self, position):
        if " moves " in position:
            position, moves = position.split(" moves ")
            moves = moves.split()
        else:
            moves = []

        if position.startswith("fen "):
            fen = position[4:]
        elif position == "startpos":
            fen = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r"

        self.load(fen)
        for move in moves:
            move = folium.ucci2move(move)
            self.makemove(move)
        self.bans = []

    def setoption(self, option):
        return
        if option.startswith('usemillisec '):
            self.usemillisec = (option[12:] == 'true')
        elif option.startswith('debug'):
            pass

    def banmoves(self, moves):
        self.bans = moves.split()

    def go(self, line):
        self.stop = False
        self.ponder = False
        self.draw = False
        self.depth = 255
        self.starttime = folium.time()
        self.mintime = self.maxtime = self.starttime + 24*60*60

        move = self.book.search(str(self), self.bans) if self.book else None
        if move:
            self.writeline("info book move: %s" % move)
            self.writeline("info book search time: %f" % (folium.time() - self.starttime))
            self.writeline("bestmove %s" % move)
            return

        if line.startswith("go ponder "):
            self.ponder = True
            line = line[10:]
        elif line.startswith("go draw "):
            self.draw = True
            line = line[8:]
        else:
            line = line[2:]

        if self.usemillisec:
            propertime = limittime = float(24*3600*1000)
        else:
            propertime = limittime = float(24*3600)

        parameters = line.split()
        if parameters:
            parameter = parameters[0]
            if parameter == "depth":
                self.ponder = False
                parameter = parameters[1]
                if parameter != "infinite":
                    self.depth = int(parameter)
            elif parameter == "time":
                propertime = limittime = totaltime = float(parameters[1])
                parameters = parameters[2:]
                while parameters:
                    parameter = parameters[0]
                    if parameter == "movestogo":
                        count = int(parameters[1])
                        propertime = totaltime/count
                        limittime = totaltime
                    elif parameter == "increment":
                        increment = int(parameters[1])
                        propertime = totaltime*0.05+increment
                        limittime = totaltime*0.5
                    parameters = parameters[2:]
        limittime = min(propertime*1.618, limittime)
        propertime = propertime*0.618
        if self.usemillisec:
            propertime = propertime * 0.001
            limittime = limittime * 0.001

        self.mintime = self.starttime + propertime
        self.maxtime = self.starttime + limittime

        move = self.search([folium.ucci2move(move) for move in self.bans])

        if move:
            self.writeline("bestmove %s" % folium.move2ucci(move))
        else:
            self.writeline("nobestmove")