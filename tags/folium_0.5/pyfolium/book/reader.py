import os
import sys
import dbhash
import cPickle
import random

import mirror

class Reader(object):
    def __init__(self):
        bookfile = os.path.join(os.path.dirname(sys.argv[0]), 'snake.book')
        self.book = dbhash.open(bookfile, 'r')
        self.mirror = mirror.Mirror()

    def flag(self, fen):
        _ = []
        for flag in self.mirror.flags:
            new= self.mirror.fen(fen, flag)
            if not new.endswith('b'):
                _.append([new, flag])
        _.sort()
        return _[0] if _[0][0] >= _[1][0] else _[1]

    def search(self, fen, bans):
        fen, flag = self.flag(fen)
        if not self.book.has_key(fen):
            return

        fen, moves = self.book.set_location(fen)
        moves = cPickle.loads(moves)
        moves = [(self.mirror.move(move, flag), moves[move]) for move in moves]
        moves = [(move, score) for move, score in moves if move not in bans]

        scores = sum(score for move, score in moves)
        idx = random.randint(0, scores-1)
        for move, score in moves:
            if score > idx:
                return move
            idx = idx - score
