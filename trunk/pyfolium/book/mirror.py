#coding=utf-8

__all__ = ['flags', 'move', 'fen']

flags = range(4)

_m1_map = dict(zip('abcdefghi0123456789', 'ihgfedcba0123456789'))
_m2_map = dict(zip('abcdefghi0123456789', 'ihgfedcba9876543210'))
def move(move, flag):
    if flag&1:
        move = ''.join(_m1_map[c] for c in move)
    if flag&2:
        move = ''.join(_m2_map[c] for c in move)
    return move

def fen(fen, flag):
    board, player = fen.split()[:2]
    player = 'b' if player is 'b' else 'r'
    if flag&1:
        board = '/'.join(''.join(reversed(line)) for line in board.split('/'))
    if flag&2:
        board = ''.join(reversed(board.swapcase()))
        player = 'r' if player is 'b' else 'b'
    return '%s %s' % (board, player)
