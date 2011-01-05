import time

import _xq

import snake.book.dictbook

class Engine(_xq.UCCI):
    def __init__(self):
        _xq.UCCI.__init__(self)
        self.book = snake.book.dictbook.dictbook()

    def run(self):
        _xq.writeline('id name folium')
        _xq.writeline('id author Wangmao Lin')
        _xq.writeline('id user folium users')
        _xq.writeline('option usemillisec type check default true')
        self.usemillisec = True

        _xq.writeline('ucciok')

        self.bans = []
        self.loop()

    def loop(self):
        while True:
            while not _xq.readable():
                time.sleep(0.001)
            line = _xq.readline()
            if line == 'isready':
                _xq.writeline('readyok')
            elif line == 'quit' or line == 'exit':
                _xq.writeline('bye')
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
            _xq.writeline("info book move: %s" % move)
            _xq.writeline("info book search time: %f" % (_xq.currenttime() - self.starttime))
            _xq.writeline("bestmove %s" % move)
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
            _xq.writeline("bestmove %s" % _xq.move2ucci(move))
        else:
            _xq.writeline("nobestmove")