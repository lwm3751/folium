#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <cassert>

typedef     char                sint8;
typedef     unsigned char       uint8;
typedef     short               sint16;
typedef     unsigned short      uint16;
typedef     int                 sint32;
typedef     unsigned int        uint32;
#ifdef _MSC_VER
typedef     __int64             sint64;
typedef     unsigned __int64    uint64;
#else
typedef     long long           sint64;
typedef     unsigned long long  uint64;
#endif

typedef     int                 sint;
typedef     unsigned int        uint;


const uint Red                  = 0UL;
const uint Black                = 1UL;
const uint Empty                = 2UL;
const uint Invaild              = 3UL;

const uint RedKing              = 0UL;
const uint RedAdvisor           = 1UL;
const uint RedBishop            = 2UL;
const uint RedRook              = 3UL;
const uint RedKnight            = 4UL;
const uint RedCannon            = 5UL;
const uint RedPawn              = 6UL;
const uint BlackKing            = 7UL;
const uint BlackAdvisor         = 8UL;
const uint BlackBishop          = 9UL;
const uint BlackRook            = 10UL;
const uint BlackKnight          = 11UL;
const uint BlackCannon          = 12UL;
const uint BlackPawn            = 13UL;
const uint EmptyPiece           = 14UL;
const uint InvaildPiece         = 15UL;

const uint RedKingFlag          = 1UL << RedKing;
const uint RedAdvisorFlag       = 1UL << RedAdvisor;
const uint RedBishopFlag        = 1UL << RedBishop;
const uint RedRookFlag          = 1UL << RedRook;
const uint RedKnightFlag        = 1UL << RedKnight;
const uint RedCannonFlag        = 1UL << RedCannon;
const uint RedPawnFlag          = 1UL << RedPawn;
const uint BlackKingFlag        = 1UL << BlackKing;
const uint BlackAdvisorFlag     = 1UL << BlackAdvisor;
const uint BlackBishopFlag      = 1UL << BlackBishop;
const uint BlackRookFlag        = 1UL << BlackRook;
const uint BlackKnightFlag      = 1UL << BlackKnight;
const uint BlackCannonFlag      = 1UL << BlackCannon;
const uint BlackPawnFlag        = 1UL << BlackPawn;
const uint EmptyFlag            = 1UL << EmptyPiece;
const uint InvaildFlag          = 1UL << InvaildPiece;
const uint KingFlag             = RedKingFlag
                                  | BlackKingFlag;
const uint AdvisorFlag          = RedAdvisorFlag
                                  | BlackAdvisorFlag;
const uint BishopFlag           = RedBishopFlag
                                  | BlackBishopFlag;
const uint RookFlag             = RedRookFlag
                                  | BlackRookFlag;
const uint KnightFlag           = RedKnightFlag
                                  | BlackKnightFlag;
const uint CannonFlag           = RedCannonFlag
                                  | BlackCannonFlag;
const uint PawnFlag             = RedPawnFlag
                                  | BlackPawnFlag;
const uint RedFlag              = RedKingFlag
                                  | RedAdvisorFlag
                                  | RedBishopFlag
                                  | RedRookFlag
                                  | RedKnightFlag
                                  | RedCannonFlag
                                  | RedPawnFlag;
const uint BlackFlag            = BlackKingFlag
                                  | BlackAdvisorFlag
                                  | BlackBishopFlag
                                  | BlackRookFlag
                                  | BlackKnightFlag
                                  | BlackCannonFlag
                                  | BlackPawnFlag;
const uint RedAndEmptyFlag      = RedFlag
                                  | EmptyFlag;
const uint BlackAndEmptyFlag    = BlackFlag
                                  | EmptyFlag;

const uint RedKingIndex         = 0UL;
const uint RedAdvisorIndex1     = 1UL;
const uint RedAdvisorIndex2     = 2UL;
const uint RedBishopIndex1      = 3UL;
const uint RedBishopIndex2      = 4UL;
const uint RedRookIndex1        = 5UL;
const uint RedRookIndex2        = 6UL;
const uint RedKnightIndex1      = 7UL;
const uint RedKnightIndex2      = 8UL;
const uint RedCannonIndex1      = 9UL;
const uint RedCannonIndex2      = 10UL;
const uint RedPawnIndex1        = 11UL;
const uint RedPawnIndex2        = 12UL;
const uint RedPawnIndex3        = 13UL;
const uint RedPawnIndex4        = 14UL;
const uint RedPawnIndex5        = 15UL;
const uint BlackKingIndex       = 16UL;
const uint BlackAdvisorIndex1   = 17UL;
const uint BlackAdvisorIndex2   = 18UL;
const uint BlackBishopIndex1    = 19UL;
const uint BlackBishopIndex2    = 20UL;
const uint BlackRookIndex1      = 21UL;
const uint BlackRookIndex2      = 22UL;
const uint BlackKnightIndex1    = 23UL;
const uint BlackKnightIndex2    = 24UL;
const uint BlackCannonIndex1    = 25UL;
const uint BlackCannonIndex2    = 26UL;
const uint BlackPawnIndex1      = 27UL;
const uint BlackPawnIndex2      = 28UL;
const uint BlackPawnIndex3      = 29UL;
const uint BlackPawnIndex4      = 30UL;
const uint BlackPawnIndex5      = 31UL;
const uint EmptyIndex           = 32UL;
const uint InvaildIndex         = 33UL;
    
const uint InvaildSquare        = 90UL;

inline uint move_src(uint32 move)
{
    return move & 0x7f;
}

inline uint move_dst(uint32 move)
{
    return (move >> 7) & 0x7f;
}
inline uint32 create_move(uint src, uint dst)
{
    assert(src < 90);
    assert(dst < 90);
    return src | (dst << 7);
}

#define trace_src move_src
#define trace_dst move_dst
inline uint trace_dst_piece(uint32 trace)
{
    return (trace >> 14) & 0x3f;
}
inline uint trace_flag(uint32 trace)
{
    return trace  >> 20;
}
inline uint32 create_trace(uint flag, uint dst_piece, uint32 move)
{
    assert (flag < 16);//4bits
    assert (dst_piece < 33);//6bits
    return (flag << 20) | (dst_piece << 14) | (move & 0x3fff);
}

#endif    //_DEFINES_H_

