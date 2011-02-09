import time

import _xq

import snake.book.dictbook

class Engine(_xq.Engine):
    def __init__(self):
        _xq.Engine.__init__(self)
        self.book = snake.book.dictbook.dictbook()

    def run(self):
        while not self.readable():
            pass
        line = self.readline()
        if line == 'ucci':
            self.ucci()
        elif line == 'perft':
            self.perft()

    def perft(self):
        def _():
            self.writeline(str(xq))
            for index, count in enumerate(perfts):
                t1 = _xq.currenttime()
                assert(xq.perft(index) == count)
                t2 = _xq.currenttime()
                if t2 - t1 > 0.1:
                    self.writeline("ply:%d\tcount:%d\tnode:%d"%(index+1, count, int(count/(t2-t1))))

        xq = _xq.XQ("r1ba1a3/4kn3/2n1b4/pNp1p1p1p/4c4/6P2/P1P2R2P/1CcC5/9/2BAKAB2 r")
        perfts = [38, 1128, 43929, 1339047, 53112976]
        _()
        xq = _xq.XQ("r2akab1r/3n5/4b3n/p1p1pRp1p/9/2P3P2/P3P3c/N2C3C1/4A4/1RBAK1B2 r")
        perfts = [58, 1651, 90744, 2605437, 140822416]
        _()
        xq = _xq.XQ("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r")
        perfts = [44, 1920, 79666, 3290240, 133312995, 5392831844]
        #_()


    def ucci(self):
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
            move = _xq.ucci2move(move)
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
        self.starttime = _xq.currenttime()
        self.mintime = self.maxtime = self.starttime + 24*60*60

        move = self.book.search(str(self), self.bans) if self.book else None
        if move:
            self.writeline("info book move: %s" % move)
            self.writeline("info book search time: %f" % (_xq.currenttime() - self.starttime))
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

        move = self.search([_xq.ucci2move(move) for move in self.bans])

        if move:
            self.writeline("bestmove %s" % _xq.move2ucci(move))
        else:
            self.writeline("nobestmove")