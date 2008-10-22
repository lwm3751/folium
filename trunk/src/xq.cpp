#include <cstring>
#include <vector>
using namespace std;
#include "xq.h"

namespace folium
{
    XQ::XQ()
    {
        clear();
    }

    XQ::XQ(XQ const &xq)
    {
        m_bitmap = xq.m_bitmap;
        memcpy(m_pieces, xq.m_pieces, 34);
        memcpy(m_squares, xq.m_squares, 91);
        m_player = xq.m_player;
    }

    bool XQ::is_legal_move(uint32 src, uint32 dst) const
    {
        if (src > 90 || dst > 90)
            return false;
        uint32 src_piece = square(src);
        //这里同时排除了src == dst
        if (piece_color(src_piece) == square_color(dst))
            return false;
        if (!(g_move_flags[dst][src] & piece_flag(src_piece)))
            return false;
        switch (piece_type(src_piece))
        {
        case RedKing:
            if (g_move_flags[dst][src] & RedPawnFlag)
                return true;
            return (square_flag(dst) & BlackKingFlag) && distance(src, dst) == 0;
        case BlackKing:
            if (g_move_flags[dst][src] & BlackPawnFlag)
                return true;
            return (square_flag(dst) & RedKingFlag) && distance(src, dst) == 0;
        case RedAdvisor:
        case BlackAdvisor:
            return true;
        case RedBishop:
        case BlackBishop:
            return square(bishop_eye(src, dst)) == EmptyIndex;
        case RedRook:
        case BlackRook:
            return distance_is_0(src, dst);
        case RedKnight:
        case BlackKnight:
            return square(knight_leg(src, dst)) == EmptyIndex;
        case RedCannon:
        case BlackCannon:
            return (distance(src, dst) + (square(dst) >> 5)) == 1;
        case RedPawn:
        case BlackPawn:
            return true;
        default:
            return false;
        }
    }

    uint XQ::status() const
    {
        if (player_in_check(player()))
            return 1;
        return 0;
    }

    uint XQ::player_in_check(uint player) const
    {
        if (player == Red)
        {
            uint kp = piece(RedKingIndex);
            assert(kp != InvaildSquare);
            return ((square_flag(square_up(kp))
                     | square_flag(square_left(kp))
                     | square_flag(square_right(kp)))
                    & BlackPawnFlag)//pawn
                   | ((square_flag(nonempty_down_1(kp))
                       | square_flag(nonempty_up_1(kp))
                       | square_flag(nonempty_left_1(kp))
                       | square_flag(nonempty_right_1(kp)))
                      & (BlackRookFlag | BlackKingFlag))//rook
                   | ((square_flag(nonempty_down_2(kp))
                       | square_flag(nonempty_up_2(kp))
                       | square_flag(nonempty_left_2(kp))
                       | square_flag(nonempty_right_2(kp)))
                      & BlackCannonFlag)//cannon
                   | ((square_flag(knight_leg(piece(BlackKnightIndex1), kp))
                       | square_flag(knight_leg(piece(BlackKnightIndex2), kp)))
                      & EmptyFlag);//knight
        }
        else
        {
            uint kp = piece(BlackKingIndex);
            assert(kp != InvaildSquare);
            return ((square_flag(square_down(kp))
                     | square_flag(square_left(kp))
                     | square_flag(square_right(kp)))
                    & RedPawnFlag)//pawn
                   | ((square_flag(nonempty_down_1(kp))
                       | square_flag(nonempty_up_1(kp))
                       | square_flag(nonempty_left_1(kp))
                       | square_flag(nonempty_right_1(kp)))
                      & (RedRookFlag | RedKingFlag))//rook
                   | ((square_flag(nonempty_down_2(kp))
                       | square_flag(nonempty_up_2(kp))
                       | square_flag(nonempty_left_2(kp))
                       | square_flag(nonempty_right_2(kp)))
                      & RedCannonFlag)//cannon
                   | ((square_flag(knight_leg(piece(RedKnightIndex1), kp))
                       | square_flag(knight_leg(piece(RedKnightIndex2), kp)))
                      & EmptyFlag);//knight
        }
    }

    bool XQ::is_good_cap(uint move)const
    {
        uint dst = move_dst(move);
        uint dst_piece = square(dst);
        if (dst_piece == EmptyIndex)
            return false;
        uint src_piece = square(move_src(move));
        if (g_simple_values[src_piece] < g_simple_values[dst_piece])
            return true;
        if (player() == Red)
        {
            //king
            {
                uint src = piece(BlackKingIndex);
                if (g_move_flags[dst][src] & BlackPawnFlag)
                    return false;
            }
            //advisor
            for (uint idx = BlackAdvisorIndex1; idx <= BlackAdvisorIndex2; ++idx)
            {
                uint src = piece(idx);
                if (g_move_flags[dst][src] & BlackAdvisorFlag)
                    return false;
            }
            //bishop
            for (uint idx = BlackBishopIndex1; idx <= BlackBishopIndex2; ++idx)
            {
                uint src = piece(idx);
                if (g_move_flags[dst][src] & BlackBishopFlag)
                    if (square(bishop_eye(src, dst)) == EmptyIndex)
                        return false;
            }
            //rook
            for (uint idx = BlackRookIndex1; idx <= BlackRookIndex2; ++idx)
            {
                uint src = piece(idx);
                if (g_move_flags[dst][src] & BlackRookFlag)
                {
                    switch (distance(src, dst))
                    {
                    case 0:
                        return false;
                    case 1:
                        if (distance(src, move_src(move)) == 0)
                            return false;
                    }
                }
            }
            //knight
            for (uint idx = BlackKnightIndex1; idx <= BlackKnightIndex2; ++idx)
            {
                uint src = piece(idx);
                if (g_move_flags[dst][src] & BlackKnightFlag)
                    if (square(knight_leg(src, dst)) == EmptyIndex)
                        return false;
            }
            //cannon
            for (uint idx = BlackCannonIndex1; idx <= BlackCannonIndex2; ++idx)
            {
                uint src = piece(idx);
                if (g_move_flags[dst][src] & BlackCannonFlag)
                {
                    switch (distance(src, dst))
                    {
                    case 1:
                        if (distance(src, move_src(move)) != 0)
                            return false;
                        break;
                    case 2:
                        if (distance(src, move_src(move)) < 2)
                            return false;
                    }
                }
            }
            //pawn
            for (uint idx = BlackPawnIndex1; idx <= BlackPawnIndex5; ++idx)
            {
                uint src = piece(idx);
                if (g_move_flags[dst][src] & BlackPawnFlag)
                    return false;
            }
        }
        else
        {
            //king
            {
                uint src = piece(RedKingIndex);
                if (g_move_flags[dst][src] & RedPawnFlag)
                    return false;
            }
            //advisor
            for (uint idx = RedAdvisorIndex1; idx <= RedAdvisorIndex2; ++idx)
            {
                uint src = piece(idx);
                if (g_move_flags[dst][src] & RedAdvisorFlag)
                    return false;
            }
            //bishop
            for (uint idx = RedBishopIndex1; idx <= RedBishopIndex2; ++idx)
            {
                uint src = piece(idx);
                if (g_move_flags[dst][src] & RedBishopFlag)
                    if (square(bishop_eye(src, dst)) == EmptyIndex)
                        return false;
            }
            //rook
            for (uint idx = RedRookIndex1; idx <= RedRookIndex2; ++idx)
            {
                uint src = piece(idx);
                if (g_move_flags[dst][src] & RedRookFlag)
                {
                    switch (distance(src, dst))
                    {
                    case 0:
                        return false;
                    case 1:
                        if (distance(src, move_src(move)) == 0)
                            return false;
                    }
                }
            }
            //knight
            for (uint idx = RedKnightIndex1; idx <= RedKnightIndex2; ++idx)
            {
                uint src = piece(idx);
                if (g_move_flags[dst][src] & RedKnightFlag)
                    if (square(knight_leg(src, dst)) == EmptyIndex)
                        return false;
            }
            //cannon
            for (uint idx = RedCannonIndex1; idx <= RedCannonIndex2; ++idx)
            {
                uint src = piece(idx);
                if (g_move_flags[dst][src] & RedCannonFlag)
                {
                    switch (distance(src, dst))
                    {
                    case 1:
                        if (distance(src, move_src(move)) != 0)
                            return false;
                        break;
                    case 2:
                        if (distance(src, move_src(move)) < 2)
                            return false;
                    }
                }
            }
            //pawn
            for (uint idx = RedPawnIndex1; idx <= RedPawnIndex5; ++idx)
            {
                uint src = piece(idx);
                if (g_move_flags[dst][src] & RedPawnFlag)
                    return false;
            }
        }
        return true;
    }

    void XQ::clear()
    {
        m_bitmap.reset();
        memset(m_pieces, InvaildSquare, 34);
        memset(m_squares, EmptyIndex, 90);
        m_squares[90] = InvaildIndex;
        m_player = Empty;
    }
}//namespace folium
