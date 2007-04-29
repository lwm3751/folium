﻿#!/usr/bin/env python
#coding=utf-8
import os
import string
from consts import *

def u(x): return SquareUps[x]
def d(x): return SquareDowns[x]
def l(x): return SquareLefts[x]
def r(x): return SquareRights[x]

SquareUps = [0]*91
SquareDowns = [0]*91
SquareLefts = [0]*91
SquareRights = [0]*91
Xs = [9]*91
Ys = [10]*91
XYs = [[90]*16 for i in range(16)]

KnightLegs = [[90]*128 for i in range(91)]
BishopEyes = [[90]*128 for i in range(91)]

def info():
    def _(x, y):
        if x < 0 or x >8 or y < 0 or y > 9:
            return 90
        return x + 9 * y
    for sq in range(90):
        #x, y = sq % 9, sq / 9
        y, x = divmod(sq, 9)
        SquareUps[sq] = _(x, y - 1)
        SquareDowns[sq] = _(x, y + 1)
        SquareLefts[sq] = _(x - 1, y)
        SquareRights[sq] = _(x + 1, y)
        Xs[sq] = x
        Ys[sq] = y
        XYs[y][x] = sq
    SquareUps[90] = 90
    SquareDowns[90] = 90
    SquareLefts[90] = 90
    SquareRights[90] = 90
info()

def leg():
    u = lambda s:SquareUps[s]
    d = lambda s:SquareDowns[s]
    l = lambda s:SquareLefts[s]
    r = lambda s:SquareRights[s]
    for src in range(90):
        leg = u(src)
        dst = l(u(leg))
        if dst != 90: KnightLegs[src][dst] = leg
        dst = r(u(leg))
        if dst != 90: KnightLegs[src][dst] = leg

        leg = d(src)
        dst = l(d(leg))
        if dst != 90: KnightLegs[src][dst] = leg
        dst = r(d(leg))
        if dst != 90: KnightLegs[src][dst] = leg

        leg = l(src)
        dst = u(l(leg))
        if dst != 90: KnightLegs[src][dst] = leg
        dst = d(l(leg))
        if dst != 90: KnightLegs[src][dst] = leg

        leg = r(src)
        dst = u(r(leg))
        if dst != 90: KnightLegs[src][dst] = leg
        dst = d(r(leg))
        if dst != 90: KnightLegs[src][dst] = leg
leg()

def eye():
    u = lambda s:SquareUps[s]
    d = lambda s:SquareDowns[s]
    l = lambda s:SquareLefts[s]
    r = lambda s:SquareRights[s]
    for src in range(90):
        if not (SquareFlags[src] & BishopFlag):
            continue
        dst = u(u(l(l(src))))
        if (SquareFlags[dst] & BishopFlag):BishopEyes[src][dst] = (src + dst)/2
        dst = u(u(r(r(src))))
        if (SquareFlags[dst] & BishopFlag):BishopEyes[src][dst] = (src + dst)/2
        dst = d(d(l(l(src))))
        if (SquareFlags[dst] & BishopFlag):BishopEyes[src][dst] = (src + dst)/2
        dst = d(d(r(r(src))))
        if (SquareFlags[dst] & BishopFlag):BishopEyes[src][dst] = (src + dst)/2
eye()

PF = [RedKingFlag]+[RedAdvisorFlag]*2+[RedBishopFlag]*2\
    +[RedRookFlag]*2+[RedKnightFlag]*2+[RedCannonFlag]*2+[RedPawnFlag]*5\
    +[BlackKingFlag]+[BlackAdvisorFlag]*2+[BlackBishopFlag]*2\
    +[BlackRookFlag]*2+[BlackKnightFlag]*2+[BlackCannonFlag]*2+[BlackPawnFlag]*5\
    +[EmptyFlag, InvaildFlag]
PT = [RedKing]+[RedAdvisor]*2+[RedBishop]*2+[RedRook]*2+[RedKnight]*2+[RedCannon]*2+[RedPawn]*5\
    +[BlackKing]+[BlackAdvisor]*2+[BlackBishop]*2+[BlackRook]*2+[BlackKnight]*2+[BlackCannon]*2+[BlackPawn]*5\
    +[EmptyType]+[InvalidType]
PC = [0]*16 + [1]*16 + [2] + [3]

def KnightMoves():
    KnightMoves = [[23130]*16 for i in range(91)]
    for sq in range(90):
        ls = KnightMoves[sq]
        leg = u(sq)
        for dst in [l(u(leg)),r(u(leg))]:
            if dst != 90:
                ls[ls.index(23130)] = (leg << 8) | dst
        leg = d(sq)
        for dst in [l(d(leg)),r(d(leg))]:
            if dst != 90:
                ls[ls.index(23130)] = (leg << 8) | dst
        leg = l(sq)
        for dst in [u(l(leg)),d(l(leg))]:
            if dst != 90:
                ls[ls.index(23130)] = (leg << 8) | dst
        leg = r(sq)
        for dst in [u(r(leg)),d(r(leg))]:
            if dst != 90:
                ls[ls.index(23130)] = (leg << 8) | dst
    return KnightMoves
KnightMoves = KnightMoves()

def RedKingPawnMoves():
    RedKingPawnMoves = [[90]*8 for i in range(91)]
    for sq in range(90):
        Moves = RedKingPawnMoves[sq]
        flag = SquareFlags[sq]
        sqs = []
        if flag & RedKingFlag:
            sqs = [i for i in [u(sq), d(sq), l(sq), r(sq)] if SquareFlags[i] & RedKingFlag]
        elif flag & RedPawnFlag:
            sqs = [i for i in [d(sq), l(sq), r(sq)] if SquareFlags[i] & RedPawnFlag]
        for sq in sqs:
            Moves[Moves.index(90)] = sq
    return RedKingPawnMoves
RedKingPawnMoves = RedKingPawnMoves()

def BlackKingPawnMoves():
    BlackKingPawnMoves = [[90]*8 for i in range(91)]
    for sq in range(90):
        Moves = BlackKingPawnMoves[sq]
        flag = SquareFlags[sq]
        sqs = []
        if flag & BlackKingFlag:
            sqs = [i for i in [u(sq), d(sq), l(sq), r(sq)] if SquareFlags[i] & BlackKingFlag]
        elif flag & BlackPawnFlag:
            sqs = [i for i in [u(sq), l(sq), r(sq)] if SquareFlags[i] & BlackPawnFlag]
        for sq in sqs:
            Moves[Moves.index(90)] = sq
    return BlackKingPawnMoves
BlackKingPawnMoves = BlackKingPawnMoves()

def AdvisorBishopMoves():
    AdvisorBishopMoves = [[90]*8 for i in range(91)]
    for sq in range(90):
        Moves = AdvisorBishopMoves[sq]
        flag = SquareFlags[sq]
        if flag & BishopFlag:
            for square in [u(u(r(r(sq)))), u(u(l(l(sq)))), d(d(r(r(sq)))), d(d(l(l(sq))))]:
                if SquareFlags[square] & BishopFlag:
                    Moves[Moves.index(90)] = square
        elif flag & AdvisorFlag:
            for square in [u(l(sq)), u(r(sq)), d(l(sq)), d(r(sq))]:
                if SquareFlags[square] & AdvisorFlag:
                    Moves[Moves.index(90)] = square
    return AdvisorBishopMoves
AdvisorBishopMoves = AdvisorBishopMoves()

def main():
    dict = {}

    dict['xs'] = d1a_str(Xs, u32)
    dict['ys'] = d1a_str(Ys, u32)
    dict['xys'] = d2a_str(XYs, u32)
    dict['ups'] = d1a_str(SquareUps, u32)
    dict['downs'] = d1a_str(SquareDowns, u32)
    dict['lefts'] = d1a_str(SquareLefts, u32)
    dict['rights'] = d1a_str(SquareRights, u32)
    dict['square_flags'] = d1a_str(SquareFlags, u32)

    dict ['ptypes'] = d1a_str(PT, u32)
    dict ['pflags'] = d1a_str(PF, u32)
    dict ['pcolors'] = d1a_str(PC, u32)

    dict ['kl'] = d2a_str(KnightLegs, u32)
    dict ['be'] = d2a_str(BishopEyes, u32)

    dict['nm'] = d2a_str(KnightMoves, u32)
    dict['rkpm'] = d2a_str(RedKingPawnMoves, u32)
    dict['bkpm'] = d2a_str(BlackKingPawnMoves, u32)
    dict['abm'] = d2a_str(AdvisorBishopMoves, u32)

    template = open(os.path.join(template_path, 'xq_data.cpp.tmpl'), 'rb').read()
    template = string.Template(template)
    path = os.path.join(folium_path, 'xq_data.cpp')
    open(path, 'wb').write(str(template.safe_substitute(dict)))

if __name__ == "__main__":
    main()
