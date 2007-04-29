#!/usr/bin/env python
#coding=utf-8
import os
import string
from consts import *
from xq_data import *

RedKingPawnValues = [0]*91
BlackKingPawnValues = [0]*91
AdvisorBishopValues = [0]*91
RedRookValues = [0]*91
BlackRookValues = [0]*91
RedKnightValues = [0]*91
BlackKnightValues = [0]*91
RedCannonValues = [0]*91
BlackCannonValues = [0]*91
def value():
    KingBaseValue = 5000
    AdvisorBaseValue = 40
    BishopBaseValue = 40
    RookBaseValue = 200
    KnightBaseValue = 88
    CannonBaseValue = 96
    PawnBaseValue = 9
    RedKingPawnPositionValues = [
        0,      0,      0,      1,      5,      1,      0,      0,      0,
        0,      0,      0,      -8,     -8,     -8,     0,      0,      0,
        0,      0,      0,      -9,     -9,     -9,     0,      0,      0,
        -2,     0,      -2,     0,      6,      0,      -2,     0,      -2,
        3,      0,      4,      0,      7,      0,      4,      0,      3,
        10,     18,     22,     35,     40,     35,     22,     18,     10,
        20,     27,     30,     40,     42,     40,     35,     27,     20,
        20,     30,     45,     55,     55,     55,     45,     30,     20,
        20,     30,     50,     65,     70,     65,     50,     30,     20,
        0,      0,      0,      2,      4,      2,      0,      0,      0,]
    RedAdvisorBishopPositionValues = [
        0,      0,      0,      0,      0,      0,      0,      0,      0,
        0,      0,      0,      0,      3,      0,      0,      0,      0,
        -2,     0,      0,      0,      3,      0,      0,      0,      -2,
        0,      0,      0,      0,      0,      0,      0,      0,      0,
        0,      0,      0,      0,      0,      0,      0,      0,      0,
        0,      0,      0,      0,      0,      0,      0,      0,      0,
        0,      0,      0,      0,      0,      0,      0,      0,      0,
        0,      0,      0,      0,      0,      0,      0,      0,      0,
        0,      0,      0,      0,      0,      0,      0,      0,      0,
        0,      0,      0,      0,      0,      0,      0,      0,      0,]
    RedRookPositionValues = [
        -6,     6,      4,      12,     0,      12,     4,      6,      -6,
        5,      8,      6,      12,     0,      12,     6,      8,      5,
        -2,     8,      4,      12,     12,     12,     4,      8,      -2,
        4,      9,      4,      12,     14,     12,     4,      9,      4,
        8,      12,     12,     14,     15,     14,     12,     12,     8,
        8,      11,     11,     14,     15,     14,     11,     11,     8,
        6,      13,     13,     16,     16,     16,     13,     13,     6,
        6,      8,      7,      14,     16,     14,     7,      8,      6,
        6,      12,     9,      16,     33,     16,     9,      12,     6,
        6,      8,      7,      13,     14,     13,     7,      8,      6,]
    RedKnightPositionValues = [
        0,      -3,     2,      0,      2,      0,      2,      -3,     0,
        -3,     2,      4,      5,      -10,    5,      4,      2,      -3,
        5,      4,      6,      7,      4,      7,      6,      4,      5,
        4,      6,      10,     7,      10,     7,      10,     6,      4,
        2,      10,     13,     14,     15,     14,     13,     10,     2,
        2,      12,     11,     15,     16,     15,     11,     12,     2,
        5,      20,     12,     19,     12,     19,     12,     20,     5,
        4,      10,     11,     15,     11,     15,     11,     10,     4,
        2,      8,      15,     9,      6,      9,      15,     8,      2,
        2,      2,      2,      8,      2,      8,      2,      2,      2,]
    RedCannonPositionValues = [
        0,      0,      1,      3,      3,      3,      1,      0,      0,
        0,      1,      2,      2,      2,      2,      2,      1,      0,
        1,      0,      4,      3,      5,      3,      4,      0,      1,
        0,      0,      0,      0,      0,      0,      0,      0,      0,
        -1,     0,      3,      0,      4,      0,      3,      0,      -1,
        0,      0,      0,      0,      4,      0,      0,      0,      0,
        0,      3,      3,      2,      4,      2,      3,      3,      0,
        1,      1,      0,      -5,     -4,     -5,     0,      1,      1,
        2,      2,      0,      -4,     -7,     -4,     0,      2,      2,
        4,      4,      0,      -5,     -6,     -5,     0,      4,      4,]
    for sq in range(90):
        flag = SquareFlags[sq]
        if flag & RedKingFlag:
            RedKingPawnValues[sq] = KingBaseValue + RedKingPawnPositionValues[sq]
            BlackKingPawnValues[89 - sq] = -RedKingPawnValues[sq]
        if flag & RedAdvisorFlag:
            AdvisorBishopValues[sq] = AdvisorBaseValue + RedAdvisorBishopPositionValues[sq]
            AdvisorBishopValues[89 - sq] = -AdvisorBishopValues[sq]
        if flag & RedBishopFlag:
            AdvisorBishopValues[sq] = BishopBaseValue + RedAdvisorBishopPositionValues[sq]
            AdvisorBishopValues[89 - sq] = -AdvisorBishopValues[sq]
        RedRookValues[sq] = RookBaseValue + RedRookPositionValues[sq]
        BlackRookValues[89 - sq] = -RedRookValues[sq]
        RedKnightValues[sq] = KnightBaseValue + RedKnightPositionValues[sq]
        BlackKnightValues[89 - sq] = -RedKnightValues[sq]
        RedCannonValues[sq] = CannonBaseValue + RedCannonPositionValues[sq]
        BlackCannonValues[89 - sq] = -RedCannonValues[sq]
        if flag & RedPawnFlag:
            RedKingPawnValues[sq] = PawnBaseValue + RedKingPawnPositionValues[sq]
            BlackKingPawnValues[89 - sq] = -RedKingPawnValues[sq]
value()

RedKingPawnLocks = [0]*91
BlackKingPawnLocks = [0]*91
AdvisorBishopLocks = [0]*91
RedRookLocks = [0]*91
BlackRookLocks = [0]*91
RedKnightLocks = [0]*91
BlackKnightLocks = [0]*91
RedCannonLocks = [0]*91
BlackCannonLocks = [0]*91

RedKingPawnKeys = [0]*91
BlackKingPawnKeys = [0]*91
AdvisorBishopKeys = [0]*91
RedRookKeys = [0]*91
BlackRookKeys = [0]*91
RedKnightKeys = [0]*91
BlackKnightKeys = [0]*91
RedCannonKeys = [0]*91
BlackCannonKeys = [0]*91
def hash():
    from random import randint, seed
    seed(51)
    for sq in range(90):
        flag = SquareFlags[sq]
        if flag & RedKingPawnFlag:
            RedKingPawnLocks[sq] = randint(0, 0x10000000000000000)
            RedKingPawnKeys[sq] = randint(0, 0x100000000)
            BlackKingPawnLocks[89 - sq] = randint(0, 0x10000000000000000)
            BlackKingPawnKeys[89 - sq] = randint(0, 0x100000000)
        if flag & AdvisorBishopFlag:
            AdvisorBishopLocks[sq] = randint(0, 0x10000000000000000)
            AdvisorBishopKeys[sq] = randint(0, 0x100000000)
        RedRookLocks[sq] = randint(0, 0x10000000000000000)
        RedRookKeys[sq] = randint(0, 0x100000000)
        BlackRookLocks[sq] = randint(0, 0x10000000000000000)
        BlackRookKeys[sq] = randint(0, 0x100000000)
        RedKnightLocks[sq] = randint(0, 0x10000000000000000)
        RedKnightKeys[sq] = randint(0, 0x100000000)
        BlackKnightLocks[sq] = randint(0, 0x10000000000000000)
        BlackKnightKeys[sq] = randint(0, 0x100000000)
        RedCannonLocks[sq] = randint(0, 0x10000000000000000)
        RedCannonKeys[sq] = randint(0, 0x100000000)
        BlackCannonLocks[sq] = randint(0, 0x10000000000000000)
        BlackCannonKeys[sq] = randint(0, 0x100000000)
hash()

def main():
    dict = {}

    dict['rkpl'] = d1a_str(RedKingPawnLocks, u64)
    dict['rkpk'] = d1a_str(RedKingPawnKeys, u32)
    dict['rkpv'] = d1a_str(RedKingPawnValues, s32)

    dict['bkpl'] = d1a_str(BlackKingPawnLocks, u64)
    dict['bkpk'] = d1a_str(BlackKingPawnKeys, u32)
    dict['bkpv'] = d1a_str(BlackKingPawnValues, s32)

    dict['abl'] = d1a_str(AdvisorBishopLocks, u64)
    dict['abk'] = d1a_str(AdvisorBishopKeys, u32)
    dict['abv'] = d1a_str(AdvisorBishopValues, s32)

    dict['rrl'] = d1a_str(RedRookLocks, u64)
    dict['rrk'] = d1a_str(RedRookKeys, u32)
    dict['rrv'] = d1a_str(RedRookValues, s32)

    dict['brl'] = d1a_str(BlackRookLocks, u64)
    dict['brk'] = d1a_str(BlackRookKeys, u32)
    dict['brv'] = d1a_str(BlackRookValues, s32)

    dict['rnl'] = d1a_str(RedKnightLocks, u64)
    dict['rnk'] = d1a_str(RedKnightKeys, u32)
    dict['rnv'] = d1a_str(RedKnightValues, s32)

    dict['bnl'] = d1a_str(BlackKnightLocks, u64)
    dict['bnk'] = d1a_str(BlackKnightKeys, u32)
    dict['bnv'] = d1a_str(BlackKnightValues, s32)

    dict['rcl'] = d1a_str(RedCannonLocks, u64)
    dict['rck'] = d1a_str(RedCannonKeys, u32)
    dict['rcv'] = d1a_str(RedCannonValues, s32)

    dict['bcl'] = d1a_str(BlackCannonLocks, u64)
    dict['bck'] = d1a_str(BlackCannonKeys, u32)
    dict['bcv'] = d1a_str(BlackCannonValues, s32)

    PF = [RedKingFlag]+[RedAdvisorFlag]*2+[RedBishopFlag]*2\
        +[RedRookFlag]*2+[RedKnightFlag]*2+[RedCannonFlag]*2+[RedPawnFlag]*5\
        +[BlackKingFlag]+[BlackAdvisorFlag]*2+[BlackBishopFlag]*2\
        +[BlackRookFlag]*2+[BlackKnightFlag]*2+[BlackCannonFlag]*2+[BlackPawnFlag]*5\
        +[EmptyFlag, InvaildFlag]
    PT = [RedKing]+[RedAdvisor]*2+[RedBishop]*2+[RedRook]*2+[RedKnight]*2+[RedCannon]*2+[RedPawn]*5\
        +[BlackKing]+[BlackAdvisor]*2+[BlackBishop]*2+[BlackRook]*2+[BlackKnight]*2+[BlackCannon]*2+[BlackPawn]*5\
        +[EmptyType]+[InvalidType]
    PC = [0]*16 + [1]*16 + [2] + [3]

    PL = ['s_red_king_pawn_locks']+['s_advisor_bishop_locks']*4+['s_red_rook_locks']*2+['s_red_knight_locks']*2\
        +['s_red_cannon_locks']*2+['s_red_king_pawn_locks']*5\
        +['s_black_king_pawn_locks']+['s_advisor_bishop_locks']*4+['s_black_rook_locks']*2+['s_black_knight_locks']*2\
        +['s_black_cannon_locks']*2+['s_black_king_pawn_locks']*5
    PK = ['s_red_king_pawn_keys']+['s_advisor_bishop_keys']*4+['s_red_rook_keys']*2+['s_red_knight_keys']*2\
        +['s_red_cannon_keys']*2+['s_red_king_pawn_keys']*5\
        +['s_black_king_pawn_keys']+['s_advisor_bishop_keys']*4+['s_black_rook_keys']*2+['s_black_knight_keys']*2\
        +['s_black_cannon_keys']*2+['s_black_king_pawn_keys']*5
    PV = ['s_red_king_pawn_values']+['s_advisor_bishop_values']*4+['s_red_rook_values']*2+['s_red_knight_values']*2\
        +['s_red_cannon_values']*2+['s_red_king_pawn_values']*5\
        +['s_black_king_pawn_values']+['s_advisor_bishop_values']*4+['s_black_rook_values']*2+['s_black_knight_values']*2\
        +['s_black_cannon_values']*2+['s_black_king_pawn_values']*5
    dict['plocks'] = d1a_str(PL, lambda x: x)
    dict['pkeys'] = d1a_str(PK, lambda x: x)
    dict['pvalues'] = d1a_str(PV, lambda x: x)
    TL = ['s_red_king_pawn_locks']+['s_advisor_bishop_locks']*2\
        +['s_red_rook_locks','s_red_knight_locks','s_red_cannon_locks','s_red_king_pawn_locks']\
        +['s_black_king_pawn_locks']+['s_advisor_bishop_locks']*2\
        +['s_black_rook_locks','s_black_knight_locks','s_black_cannon_locks','s_black_king_pawn_locks']
    TK = ['s_red_king_pawn_keys']+['s_advisor_bishop_keys']*2\
        +['s_red_rook_keys','s_red_knight_keys','s_red_cannon_keys','s_red_king_pawn_keys']\
        +['s_black_king_pawn_keys']+['s_advisor_bishop_keys']*2\
        +['s_black_rook_keys','s_black_knight_keys','s_black_cannon_keys','s_black_king_pawn_keys']
    TV = ['s_red_king_pawn_values']+['s_advisor_bishop_values']*2\
        +['s_red_rook_values','s_red_knight_values','s_red_cannon_values','s_red_king_pawn_values']\
        +['s_black_king_pawn_values']+['s_advisor_bishop_values']*2\
        +['s_black_rook_values','s_black_knight_values','s_black_cannon_values','s_black_king_pawn_values']
    dict['tlocks'] = d1a_str(TL, lambda x: x)
    dict['tkeys'] = d1a_str(TK, lambda x: x)
    dict['tvalues'] = d1a_str(TV, lambda x: x)

    #template = string.Template(template)
    template = open(os.path.join(template_path, 'engine_data.cpp.tmpl'), 'rb').read()
    template = string.Template(template)
    path = os.path.join(folium_path, 'engine_data.cpp')
    open(path, 'wb').write(str(template.safe_substitute(dict)))
if __name__ == "__main__":
    main()