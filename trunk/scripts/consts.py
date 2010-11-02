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

def _(x, y): return x + y * 9
RedKingCoordinates = [_(x, y) for x in [3, 4, 5] for y in [0, 1, 2]]
RedAdvisorCoordinates = [_(4, 1)] + [_(x, y) for x in [3, 5] for y in [0, 2]]
RedBishopCoordinates = [_(x, y) for x in [0, 4, 8] for y in [2]] + [_(x, y) for x in [2, 6] for y in [0, 4]]
RedRookCoordinates = range(90)
RedKnightCoordinates = range(90)
RedCannonCoordinates = range(90)
RedPawnCoordinates = [_(x, y) for x in [0, 2, 4, 6, 8] for y in [3, 4]]
RedPawnCoordinates.extend(range(45, 90))

BlackKingCoordinates = [89 - sq for sq in RedKingCoordinates]
BlackAdvisorCoordinates = [89 - sq for sq in RedAdvisorCoordinates]
BlackBishopCoordinates = [89 - sq for sq in RedBishopCoordinates]
BlackRookCoordinates = [89 - sq for sq in RedRookCoordinates]
BlackKnightCoordinates = [89 - sq for sq in RedKnightCoordinates]
BlackCannonCoordinates = [89 - sq for sq in RedCannonCoordinates]
BlackPawnCoordinates = [89 - sq for sq in RedPawnCoordinates]

def CoordinateFlags():
    CoordinateFlags = [0]*91
    for sq in RedKingCoordinates:
        CoordinateFlags[sq] |= RedKingFlag
        CoordinateFlags[89 - sq] |= BlackKingFlag
    for sq in RedAdvisorCoordinates:
        CoordinateFlags[sq] |= RedAdvisorFlag
        CoordinateFlags[89 - sq] |= BlackAdvisorFlag
    for sq in RedBishopCoordinates:
        CoordinateFlags[sq] |= RedBishopFlag
        CoordinateFlags[89 - sq] |= BlackBishopFlag
    for sq in RedPawnCoordinates:
        CoordinateFlags[sq] |= RedPawnFlag
        CoordinateFlags[89 - sq] |= BlackPawnFlag
    for sq in range(90):
        CoordinateFlags[sq] |= RedRookFlag
        CoordinateFlags[sq] |= RedKnightFlag
        CoordinateFlags[sq] |= RedCannonFlag
        CoordinateFlags[sq] |= BlackRookFlag
        CoordinateFlags[sq] |= BlackKnightFlag
        CoordinateFlags[sq] |= BlackCannonFlag
        CoordinateFlags[sq] |= EmptyFlag
    CoordinateFlags[90] |= InvaildFlag
    return CoordinateFlags
CoordinateFlags = CoordinateFlags()

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
folium_path = os.path.join(work_path, 'xq')
template_path = os.path.join(script_path, 'template')
if not os.path.exists(template_path):
    os.mkdir(template_path)

if __name__ == "__main__":
    print work_path
    print script_path
    print folium_path
