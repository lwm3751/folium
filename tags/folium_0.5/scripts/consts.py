RedKing = 0;
RedAdvisor = 1;
RedBishop = 2;
RedRook = 3;
RedKnight = 4;
RedCannon = 5;
RedPawn = 6;
BlackKing = 7;
BlackAdvisor = 8;
BlackBishop = 9;
BlackRook = 10;
BlackKnight = 11;
BlackCannon = 12;
BlackPawn = 13;
EmptyType = 14;
InvalidType = 15;

RedKingFlag = 1 << 0;
RedAdvisorFlag = 1 << 1;
RedBishopFlag = 1 << 2;
RedRookFlag = 1 << 3;
RedKnightFlag = 1 << 4;
RedCannonFlag = 1 << 5;
RedPawnFlag = 1 << 6;
BlackKingFlag = 1 << 7;
BlackAdvisorFlag = 1 << 8;
BlackBishopFlag = 1 << 9;
BlackRookFlag = 1 << 10;
BlackKnightFlag = 1 << 11;
BlackCannonFlag = 1 << 12;
BlackPawnFlag = 1 << 13;
EmptyFlag = 1 << 14;
InvaildFlag = 1 << 15;

AdvisorFlag = RedAdvisorFlag | BlackAdvisorFlag
BishopFlag = RedBishopFlag | BlackBishopFlag
RedKingPawnFlag = RedKingFlag | RedPawnFlag
AdvisorBishopFlag = RedAdvisorFlag | RedBishopFlag | BlackAdvisorFlag | BlackBishopFlag

RedKingSquares = [x + y * 9 for x, y in [(3, 0), (4, 0), (5, 0), (3, 1), (4, 1), (5, 1), (3, 2), (4, 2), (5, 2)]]
RedAdvisorSquares = [x + y * 9 for x, y in [(3,0), (5,0), (4, 1), (3,2), (5,2)]]
RedBishopSquares = [x + y * 9 for x, y in [(2, 0), (6, 0), (0, 2), (4, 2), (8, 2), (2, 4), (6, 4)]]
RedRookSquares = range(90)
RedKnightSquares = range(90)
RedCannonSquares = range(90)
RedPawnSquares = [x + y * 9 for x, y in [(0, 3), (2, 3), (4, 3), (6, 3), (8, 3), (0, 4), (2, 4), (4, 4), (6, 4), (8, 4)]]
RedPawnSquares.extend(range(45, 90))

BlackKingSquares = [89 - sq for sq in RedKingSquares]
BlackAdvisorSquares = [89 - sq for sq in RedAdvisorSquares]
BlackBishopSquares = [89 - sq for sq in RedBishopSquares]
BlackRookSquares = [89 - sq for sq in RedRookSquares]
BlackKnightSquares = [89 - sq for sq in RedKnightSquares]
BlackCannonSquares = [89 - sq for sq in RedCannonSquares]
BlackPawnSquares = [89 - sq for sq in RedPawnSquares]

def SquareFlags():
    SquareFlags = [0]*91
    for sq in RedKingSquares:
        SquareFlags[sq] |= RedKingFlag
        SquareFlags[89 - sq] |= BlackKingFlag
    for sq in RedAdvisorSquares:
        SquareFlags[sq] |= RedAdvisorFlag
        SquareFlags[89 - sq] |= BlackAdvisorFlag
    for sq in RedBishopSquares:
        SquareFlags[sq] |= RedBishopFlag
        SquareFlags[89 - sq] |= BlackBishopFlag
    for sq in RedPawnSquares:
        SquareFlags[sq] |= RedPawnFlag
        SquareFlags[89 - sq] |= BlackPawnFlag
    for sq in range(90):
        SquareFlags[sq] |= RedRookFlag
        SquareFlags[sq] |= RedKnightFlag
        SquareFlags[sq] |= RedCannonFlag
        SquareFlags[sq] |= BlackRookFlag
        SquareFlags[sq] |= BlackKnightFlag
        SquareFlags[sq] |= BlackCannonFlag
        SquareFlags[sq] |= EmptyFlag
    SquareFlags[90] |= InvaildFlag
    return SquareFlags
SquareFlags = SquareFlags()

def u64(i):
    return str(i)+'ULL'
def u32(i):
    return str(i)+'UL'
def s32(i):
    return str(i)+'L'

def d1a_str(array_1d, func):
    array_1d = [func(i) for i in array_1d]
    return ', '.join(array_1d)

def d2a_str(array_2d, func):
    array_2d = ['{%s}'%d1a_str(array_1d, func) for array_1d in array_2d]
    return ',\n'.join(array_2d)

import os
script_path = os.path.abspath(os.path.dirname(__file__))
work_path = os.path.dirname(script_path)
folium_path = os.path.join(work_path, 'cpp', 'folium')
template_path = os.path.join(script_path, 'template')
if not os.path.exists(template_path):
    os.mkdir(template_path)

if __name__ == "__main__":
    print work_path
    print script_path
    print folium_path
