import random

import _xq

class Book(object):
    def readflag(self, fen):
        _ = []
        for flag in range(4):
            new= _xq.mirror4fen(fen, flag)
            if not new.endswith('b'):
                _.append([new, flag])
        _.sort()
        return _[0] if _[0][0] >= _[1][0] else _[1]
    def search(self, fen, bans):
        fen, flag = self.readflag(fen)
        moves = self.readmoves(fen)
        moves = [(_xq.mirror4uccimove(move, flag), moves[move]) for move in moves]
        moves = [(move, score) for move, score in moves if move not in bans]
        scores = sum(score for move, score in moves)
        if scores == 0:
            return
        idx = random.randint(0, scores-1)
        for move, score in moves:
            if score > idx:
                return move
            idx = idx - score
    def addflag(self, fen, move):
        _ = []
        for flag in range(4):
            nf, nm = _xq.mirror4fen(fen, flag), _xq.mirror4uccimove(move, flag)
            if not nf.endswith('b'):
                _.append([nf, nm, flag])
        return sorted(_)[1]

