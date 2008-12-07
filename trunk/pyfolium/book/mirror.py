#coding=utf-8

class Mirror(object):
    flags = range(4)
    def __init__(self):
        object.__init__(self)
        self.m1_map = dict(zip('abcdefghi0123456789', 'ihgfedcba0123456789'))
        self.m2_map = dict(zip('abcdefghi0123456789', 'ihgfedcba9876543210'))
    def move(self, move, flag):
        if flag&1:
            move = ''.join(self.m1_map[c] for c in move)
        if flag&2:
            move = ''.join(self.m2_map[c] for c in move)
        return move
    def fen(self, fen, flag):
        board, player = fen.split()[:2]
        player = 'b' if player is 'b' else 'r'
        if flag&1:
            board = '/'.join(''.join(reversed(line)) for line in board.split('/'))
        if flag&2:
            board = ''.join(reversed(board.swapcase()))
            player = 'r' if player is 'b' else 'b'
        return '%s %s' % (board, player)