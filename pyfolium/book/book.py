import random

import mirror

class Book(object):
    def readflag(self, fen):
        _ = []
        for flag in mirror.flags:
            new= mirror.fen(fen, flag)
            if not new.endswith('b'):
                _.append([new, flag])
        _.sort()
        return _[0] if _[0][0] >= _[1][0] else _[1]
    def search(self, fen, bans):
        fen, flag = self.readflag(fen)
        moves = self.readmoves(fen)
        moves = [(mirror.move(move, flag), moves[move]) for move in moves]
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
        for flag in mirror.flags:
            nf, nm = mirror.fen(fen, flag), mirror.move(move, flag)
            if not nf.endswith('b'):
                _.append([nf, nm, flag])
        return sorted(_)[1]
        moves = fens.setdefault(fen, {})
        moves[move] = moves.get(move, 0) + int(score)

