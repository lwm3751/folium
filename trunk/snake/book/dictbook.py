import os
import sys

import book

class dictbook(book.Book):
    def __init__(self):
        book.Book.__init__(self)
        self.load()
    def load(self):
        bookfile = os.path.join(os.path.dirname(sys.argv[0]), 'book.dict')
        self.dict = {}
        for line in open(bookfile):
            _ = line.split('_')
            fen = _[0]
            for s in _[1:]:
                move, score = s.split(':')
                self.add(fen, move, score)
    def add(self, fen, move, score):
        fen, move, flag = self.addflag(fen, move)
        moves = self.dict.setdefault(fen, {})
        moves[move] = moves.get(move, 0) + int(score)

    def readmoves(self, fen):
        return self.dict.get(fen, {})