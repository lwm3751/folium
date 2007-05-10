#include "movelist.h"
#include "xq.h"
#include "xq_data.h"
#include "history.h"

void XQ::generate_moves(MoveList &ml) const
{
    register uint own = player();
    register uint opp = 1 - own;
    register uint idx;
    register uint src, dst;
    ml.clear();
    if (own == Red)
    {
        idx = RedKingIndex;
        //red king
        src = piece(RedKingIndex);
        const uint8 *pm = g_red_king_pawn_moves[src];
        dst = *pm++;
        while (dst != InvaildSquare)
        {
            if (piece_color(square(dst)) != own)
                ml.push(src, dst);
            dst = *pm++;
        }
        //red pawn
        for (uint i = RedPawnIndex1; i <= RedPawnIndex5; ++i)
        {
            src = piece(i);
            if (src == InvaildSquare)
                continue;
            pm = g_red_king_pawn_moves[src];
            dst = *pm++;
            while (dst != InvaildSquare)
            {
                if (piece_color(square(dst)) != own)
                    ml.push(src, dst);
                dst = *pm++;
            }
        }
    }
    else
    {
        idx = BlackKingIndex;
        //black king
        src = piece(BlackKingIndex);
        const uint8 *pm = g_black_king_pawn_moves[src];
        dst = *pm++;
        while (dst != InvaildSquare)
        {
            if (piece_color(square(dst)) != own)
                ml.push(src, dst);
            dst = *pm++;
        }
        //black pawn
        for (uint i = BlackPawnIndex1; i <= BlackPawnIndex5; ++i)
        {
            src = piece(i);
            if (src == InvaildSquare)
                continue;
            pm = g_black_king_pawn_moves[src];
            dst = *pm++;
            while (dst != InvaildSquare)
            {
                if (piece_color(square(dst)) != own)
                    ml.push(src, dst);
                dst = *pm++;
            }
        }
    }
    //advisor
    for(uint i = 0; i < 2; ++i)
    {
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        const uint8 *pm = g_advisor_bishop_moves[src];
        dst = *pm++;
        while (dst != InvaildSquare)
        {
            if (piece_color(square(dst)) != own)
                ml.push(src, dst);
            dst = *pm++;
        }
    }
    //bishop
    for(uint i = 0; i < 2; ++i)
    {
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        const uint8 *pm = g_advisor_bishop_moves[src];
        dst = *pm++;
        while (dst != InvaildSquare)
        {
            if (piece_color(square(dst)) != own && square((dst + src) >> 1) == EmptyIndex)
                ml.push(src, dst);
            dst = *pm++;
        }
    }
    //rook
    for(uint i = 0; i < 2; ++i)
    {
        uint tmp;
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        uint sx = square_x(src);
        uint sy = square_y(src);

        const uint32 &xinfo = m_bitlines.xinfo(sx, sy);
        dst = xy_square(LineInfo::get_prev_1(xinfo), sy);
        if (piece_color(square(dst)) == opp)
            ml.push(src, dst);
        for (tmp = dst, dst = square_left(src);
            dst != tmp;
            dst = square_left(dst))
        {
            ml.push(src, dst);
        }
        dst = xy_square(LineInfo::get_next_1(xinfo), sy);
        if (piece_color(square(dst)) == opp)
            ml.push(src, dst);
        for (tmp = dst, dst = square_right(src);
            dst != tmp;
            dst = square_right(dst))
        {
            ml.push(src, dst);
        }
        const uint32 &yinfo = m_bitlines.yinfo(sx, sy);
        dst = xy_square(sx, LineInfo::get_prev_1(yinfo));
        if (piece_color(square(dst)) == opp)
            ml.push(src, dst);
        for (tmp = dst, dst = square_up(src);
            dst != tmp;
            dst = square_up(dst))
        {
            ml.push(src, dst);
        }
        dst = xy_square(sx, LineInfo::get_next_1(yinfo));
        if (piece_color(square(dst)) == opp)
            ml.push(src, dst);
        for (tmp = dst, dst = square_down(src);
            dst != tmp;
            dst = square_down(dst))
        {
            ml.push(src, dst);
        }
    }
    //knight
    for(uint i = 0; i < 2; ++i)
    {
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        const uint16 *pm = g_kinght_moves[src];
        dst = *pm++;
        //23130 = ((InvaildSquare << 8) | InvaildSquare)
        while (dst != 23130UL)
        {
            uint leg = (dst & 0xff00) >> 8;
            dst &= 0xff;
            if (square(leg) == EmptyIndex && piece_color(square(dst)) != own)
                ml.push(src, dst);
            dst = *pm++;
        }
    }
    //cannon
    for(uint i = 0; i < 2; ++i)
    {
        uint tmp;
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        uint sx = square_x(src);
        uint sy = square_y(src);

        const uint32 &xinfo = m_bitlines.xinfo(sx, sy);
        dst = xy_square(LineInfo::get_prev_2(xinfo), sy);
        if (piece_color(square(dst)) == opp)
            ml.push(src, dst);
        for (tmp = xy_square(LineInfo::get_prev_1(xinfo), sy), dst = square_left(src);
            dst != tmp;
            dst = square_left(dst))
        {
            ml.push(src, dst);
        }
        dst = xy_square(LineInfo::get_next_2(xinfo), sy);
        if (piece_color(square(dst)) == opp)
            ml.push(src, dst);
        for (tmp = xy_square(LineInfo::get_next_1(xinfo), sy), dst = square_right(src);
            dst != tmp;
            dst = square_right(dst))
        {
            ml.push(src, dst);
        }

        const uint32 &yinfo = m_bitlines.yinfo(sx, sy);
        dst = xy_square(sx, LineInfo::get_prev_2(yinfo));
        if (piece_color(square(dst)) == opp)
            ml.push(src, dst);
        for (tmp = xy_square(sx, LineInfo::get_prev_1(yinfo)), dst = square_up(src);
            dst != tmp;
            dst = square_up(dst))
        {
            ml.push(src, dst);
        }

        dst = xy_square(sx, LineInfo::get_next_2(yinfo));
        if (piece_color(square(dst)) == opp)
            ml.push(src, dst);
        for (tmp = xy_square(sx, LineInfo::get_next_1(yinfo)), dst = square_down(src);
            dst != tmp;
            dst = square_down(dst))
        {
            ml.push(src, dst);
        }
    }
}

void XQ::generate_capture_moves(MoveList &ml) const
{
    register uint own = player();
    register uint opp = 1 - own;
    register uint idx;
    register uint src, dst;
    ml.clear();
    if (own == Red)
    {
        idx = RedKingIndex;
        //red king
        src = piece(RedKingIndex);
        const uint8 *pm = g_red_king_pawn_moves[src];
        dst = *pm++;
        while (dst != InvaildSquare)
        {
            if (piece_color(square(dst)) == opp)
                ml.push(src, dst);
            dst = *pm++;
        }
        //red pawn
        for (uint i = RedPawnIndex1; i <= RedPawnIndex5; ++i)
        {
            src = piece(i);
            if (src == InvaildSquare)
                continue;
            pm = g_red_king_pawn_moves[src];
            dst = *pm++;
            while (dst != InvaildSquare)
            {
                if (piece_color(square(dst)) == opp)
                    ml.push(src, dst);
                dst = *pm++;
            }
        }
    }
    else
    {
        idx = BlackKingIndex;
        //black king
        src = piece(BlackKingIndex);
        const uint8 *pm = g_black_king_pawn_moves[src];
        dst = *pm++;
        while (dst != InvaildSquare)
        {
            if (piece_color(square(dst)) == opp)
                ml.push(src, dst);
            dst = *pm++;
        }
        //black pawn
        for (uint i = BlackPawnIndex1; i <= BlackPawnIndex5; ++i)
        {
            src = piece(i);
            if (src == InvaildSquare)
                continue;
            pm = g_black_king_pawn_moves[src];
            dst = *pm++;
            while (dst != InvaildSquare)
            {
                if (piece_color(square(dst)) == opp)
                    ml.push(src, dst);
                dst = *pm++;
            }
        }
    }
    //advisor
    for(uint i = 0; i < 2; ++i)
    {
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        const uint8 *pm = g_advisor_bishop_moves[src];
        dst = *pm++;
        while (dst != InvaildSquare)
        {
            if (piece_color(square(dst)) == opp)
                ml.push(src, dst);
            dst = *pm++;
        }
    }
    //bishop
    for(uint i = 0; i < 2; ++i)
    {
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        const uint8 *pm = g_advisor_bishop_moves[src];
        dst = *pm++;
        while (dst != InvaildSquare)
        {
            if (piece_color(square(dst)) == opp && square((dst + src) >> 1) == EmptyIndex)
                ml.push(src, dst);
            dst = *pm++;
        }
    }
    //rook
    for(uint i = 0; i < 2; ++i)
    {
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        uint sx = square_x(src);
        uint sy = square_y(src);

        const uint32 &xinfo = m_bitlines.xinfo(sx, sy);
        dst = xy_square(LineInfo::get_prev_1(xinfo), sy);
        if (piece_color(square(dst)) == opp)
            ml.push(src, dst);
        dst = xy_square(LineInfo::get_next_1(xinfo), sy);
        if (piece_color(square(dst)) == opp)
            ml.push(src, dst);
        const uint32 &yinfo = m_bitlines.yinfo(sx, sy);
        dst = xy_square(sx, LineInfo::get_prev_1(yinfo));
        if (piece_color(square(dst)) == opp)
            ml.push(src, dst);
        dst = xy_square(sx, LineInfo::get_next_1(yinfo));
        if (piece_color(square(dst)) == opp)
            ml.push(src, dst);
    }
    //knight
    for(uint i = 0; i < 2; ++i)
    {
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        const uint16 *pm = g_kinght_moves[src];
        dst = *pm++;
        //23130 = ((InvaildSquare << 8) | InvaildSquare)
        while (dst != 23130UL)
        {
            uint leg = (dst & 0xff00) >> 8;
            dst &= 0xff;
            if (square(leg) == EmptyIndex && piece_color(square(dst)) == opp)
                ml.push(src, dst);
            dst = *pm++;
        }
    }
    //cannon
    for(uint i = 0; i < 2; ++i)
    {
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        uint sx = square_x(src);
        uint sy = square_y(src);

        const uint32 &xinfo = m_bitlines.xinfo(sx, sy);
        dst = xy_square(LineInfo::get_prev_2(xinfo), sy);
        if (piece_color(square(dst)) == opp)
            ml.push(src, dst);
        dst = xy_square(LineInfo::get_next_2(xinfo), sy);
        if (piece_color(square(dst)) == opp)
            ml.push(src, dst);

        const uint32 &yinfo = m_bitlines.yinfo(sx, sy);
        dst = xy_square(sx, LineInfo::get_prev_2(yinfo));
        if (piece_color(square(dst)) == opp)
            ml.push(src, dst);
        dst = xy_square(sx, LineInfo::get_next_2(yinfo));
        if (piece_color(square(dst)) == opp)
            ml.push(src, dst);
    }
}
void XQ::generate_moves(MoveList &ml, const History& history) const
{
    register uint own = player();
    register uint opp = 1 - own;
    register uint idx;
    register uint src, dst;
    ml.clear();
    if (own == Red)
    {
        idx = RedKingIndex;
        //red king
        src = piece(RedKingIndex);
        const uint8 *pm = g_red_king_pawn_moves[src];
        dst = *pm++;
        while (dst != InvaildSquare)
        {
            if (piece_color(square(dst)) != own)
                ml.push(history.move(src, dst, RedKingIndex, square(dst)));
            dst = *pm++;
        }
        //red pawn
        for (uint i = RedPawnIndex1; i <= RedPawnIndex5; ++i)
        {
            src = piece(i);
            if (src == InvaildSquare)
                continue;
            pm = g_red_king_pawn_moves[src];
            dst = *pm++;
            while (dst != InvaildSquare)
            {
                if (piece_color(square(dst)) != own)
                    ml.push(history.move(src, dst, i, square(dst)));
                dst = *pm++;
            }
        }
    }
    else
    {
        idx = BlackKingIndex;
        //black king
        src = piece(BlackKingIndex);
        const uint8 *pm = g_black_king_pawn_moves[src];
        dst = *pm++;
        while (dst != InvaildSquare)
        {
            if (piece_color(square(dst)) != own)
                ml.push(history.move(src, dst, BlackKingIndex, square(dst)));
            dst = *pm++;
        }
        //black pawn
        for (uint i = BlackPawnIndex1; i <= BlackPawnIndex5; ++i)
        {
            src = piece(i);
            if (src == InvaildSquare)
                continue;
            pm = g_black_king_pawn_moves[src];
            dst = *pm++;
            while (dst != InvaildSquare)
            {
                if (piece_color(square(dst)) != own)
                    ml.push(history.move(src, dst, i, square(dst)));
                dst = *pm++;
            }
        }
    }
    //advisor
    for(uint i = 0; i < 2; ++i)
    {
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        const uint8 *pm = g_advisor_bishop_moves[src];
        dst = *pm++;
        while (dst != InvaildSquare)
        {
            if (piece_color(square(dst)) != own)
                ml.push(history.move(src, dst, idx, square(dst)));
            dst = *pm++;
        }
    }
    //bishop
    for(uint i = 0; i < 2; ++i)
    {
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        const uint8 *pm = g_advisor_bishop_moves[src];
        dst = *pm++;
        while (dst != InvaildSquare)
        {
            if (piece_color(square(dst)) != own && square((dst + src) >> 1) == EmptyIndex)
                ml.push(history.move(src, dst, idx, square(dst)));
            dst = *pm++;
        }
    }
    //rook
    for(uint i = 0; i < 2; ++i)
    {
        uint tmp;
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        uint sx = square_x(src);
        uint sy = square_y(src);

        const uint32 &xinfo = m_bitlines.xinfo(sx, sy);
        dst = xy_square(LineInfo::get_prev_1(xinfo), sy);
        if (piece_color(square(dst)) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        for (tmp = dst, dst = square_left(src);
            dst != tmp;
            dst = square_left(dst))
            ml.push(history.move(src, dst, idx, EmptyIndex));
        dst = xy_square(LineInfo::get_next_1(xinfo), sy);
        if (piece_color(square(dst)) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        for (tmp = dst, dst = square_right(src);
            dst != tmp;
            dst = square_right(dst))
            ml.push(history.move(src, dst, idx, EmptyIndex));
        const uint32 &yinfo = m_bitlines.yinfo(sx, sy);
        dst = xy_square(sx, LineInfo::get_prev_1(yinfo));
        if (piece_color(square(dst)) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        for (tmp = dst, dst = square_up(src);
            dst != tmp;
            dst = square_up(dst))
            ml.push(history.move(src, dst, i, EmptyIndex));
        dst = xy_square(sx, LineInfo::get_next_1(yinfo));
        if (piece_color(square(dst)) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        for (tmp = dst, dst = square_down(src);
            dst != tmp;
            dst = square_down(dst))
            ml.push(history.move(src, dst, idx, EmptyIndex));
    }
    //knight
    for(uint i = 0; i < 2; ++i)
    {
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        const uint16 *pm = g_kinght_moves[src];
        dst = *pm++;
        //23130 = ((InvaildSquare << 8) | InvaildSquare)
        while (dst != 23130UL)
        {
            uint leg = (dst & 0xff00) >> 8;
            dst &= 0xff;
            if (square(leg) == EmptyIndex && piece_color(square(dst)) != own)
                ml.push(history.move(src, dst, idx, square(dst)));
            dst = *pm++;
        }
    }
    //cannon
    for(uint i = 0; i < 2; ++i)
    {
        uint tmp;
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        uint sx = square_x(src);
        uint sy = square_y(src);

        const uint32 &xinfo = m_bitlines.xinfo(sx, sy);
        dst = xy_square(LineInfo::get_prev_2(xinfo), sy);
        if (piece_color(square(dst)) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        for (tmp = xy_square(LineInfo::get_prev_1(xinfo), sy), dst = square_left(src);
            dst != tmp;
            dst = square_left(dst))
            ml.push(history.move(src, dst, idx, EmptyIndex));
        dst = xy_square(LineInfo::get_next_2(xinfo), sy);
        if (piece_color(square(dst)) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        for (tmp = xy_square(LineInfo::get_next_1(xinfo), sy), dst = square_right(src);
            dst != tmp;
            dst = square_right(dst))
            ml.push(history.move(src, dst, idx, Empty));

        const uint32 &yinfo = m_bitlines.yinfo(sx, sy);
        dst = xy_square(sx, LineInfo::get_prev_2(yinfo));
        if (piece_color(square(dst)) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        for (tmp = xy_square(sx, LineInfo::get_prev_1(yinfo)), dst = square_up(src);
            dst != tmp;
            dst = square_up(dst))
            ml.push(history.move(src, dst, idx, Empty));

        dst = xy_square(sx, LineInfo::get_next_2(yinfo));
        if (piece_color(square(dst)) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        for (tmp = xy_square(sx, LineInfo::get_next_1(yinfo)), dst = square_down(src);
            dst != tmp;
            dst = square_down(dst))
            ml.push(history.move(src, dst, idx, Empty));
    }
}

void XQ::generate_capture_moves(MoveList &ml, const History& history) const
{
    register uint own = player();
    register uint opp = 1 - own;
    register uint idx;
    register uint src, dst;
    ml.clear();
    if (own == Red)
    {
        idx = RedKingIndex;
        //red king
        src = piece(RedKingIndex);
        const uint8 *pm = g_red_king_pawn_moves[src];
        dst = *pm++;
        while (dst != InvaildSquare)
        {
            if (piece_color(square(dst)) == opp)
                ml.push(history.move(src, dst, RedKingIndex, square(dst)));
            dst = *pm++;
        }
        //red pawn
        for (uint i = RedPawnIndex1; i <= RedPawnIndex5; ++i)
        {
            src = piece(i);
            if (src == InvaildSquare)
                continue;
            pm = g_red_king_pawn_moves[src];
            dst = *pm++;
            while (dst != InvaildSquare)
            {
                if (piece_color(square(dst)) == opp)
                    ml.push(history.move(src, dst, i, square(dst)));
                dst = *pm++;
            }
        }
    }
    else
    {
        idx = BlackKingIndex;
        //black king
        src = piece(BlackKingIndex);
        const uint8 *pm = g_black_king_pawn_moves[src];
        dst = *pm++;
        while (dst != InvaildSquare)
        {
            if (piece_color(square(dst)) == opp)
                ml.push(history.move(src, dst, idx, square(dst)));
            dst = *pm++;
        }
        //black pawn
        for (uint i = BlackPawnIndex1; i <= BlackPawnIndex5; ++i)
        {
            src = piece(i);
            if (src == InvaildSquare)
                continue;
            pm = g_black_king_pawn_moves[src];
            dst = *pm++;
            while (dst != InvaildSquare)
            {
                if (piece_color(square(dst)) == opp)
                    ml.push(history.move(src, dst, idx, square(dst)));
                dst = *pm++;
            }
        }
    }
    //advisor
    for(uint i = 0; i < 2; ++i)
    {
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        const uint8 *pm = g_advisor_bishop_moves[src];
        dst = *pm++;
        while (dst != InvaildSquare)
        {
            if (piece_color(square(dst)) == opp)
                ml.push(history.move(src, dst, idx, square(dst)));
            dst = *pm++;
        }
    }
    //bishop
    for(uint i = 0; i < 2; ++i)
    {
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        const uint8 *pm = g_advisor_bishop_moves[src];
        dst = *pm++;
        while (dst != InvaildSquare)
        {
            if (piece_color(square(dst)) == opp && square((dst + src) >> 1) == EmptyIndex)
                ml.push(history.move(src, dst, idx, square(dst)));
            dst = *pm++;
        }
    }
    //rook
    for(uint i = 0; i < 2; ++i)
    {
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        uint sx = square_x(src);
        uint sy = square_y(src);

        const uint32 &xinfo = m_bitlines.xinfo(sx, sy);
        dst = xy_square(LineInfo::get_prev_1(xinfo), sy);
        if (piece_color(square(dst)) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        dst = xy_square(LineInfo::get_next_1(xinfo), sy);
        if (piece_color(square(dst)) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        const uint32 &yinfo = m_bitlines.yinfo(sx, sy);
        dst = xy_square(sx, LineInfo::get_prev_1(yinfo));
        if (piece_color(square(dst)) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        dst = xy_square(sx, LineInfo::get_next_1(yinfo));
        if (piece_color(square(dst)) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
    }
    //knight
    for(uint i = 0; i < 2; ++i)
    {
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        const uint16 *pm = g_kinght_moves[src];
        dst = *pm++;
        //23130 = ((InvaildSquare << 8) | InvaildSquare)
        while (dst != 23130UL)
        {
            uint leg = (dst & 0xff00) >> 8;
            dst &= 0xff;
            if (square(leg) == EmptyIndex && piece_color(square(dst)) == opp)
                ml.push(history.move(src, dst, idx, square(dst)));
            dst = *pm++;
        }
    }
    //cannon
    for(uint i = 0; i < 2; ++i)
    {
        ++idx;
        src = piece(idx);
        if (src == InvaildSquare)
            continue;
        uint sx = square_x(src);
        uint sy = square_y(src);

        const uint32 &xinfo = m_bitlines.xinfo(sx, sy);
        dst = xy_square(LineInfo::get_prev_2(xinfo), sy);
        if (piece_color(square(dst)) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        dst = xy_square(LineInfo::get_next_2(xinfo), sy);
        if (piece_color(square(dst)) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));

        const uint32 &yinfo = m_bitlines.yinfo(sx, sy);
        dst = xy_square(sx, LineInfo::get_prev_2(yinfo));
        if (piece_color(square(dst)) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        dst = xy_square(sx, LineInfo::get_next_2(yinfo));
        if (piece_color(square(dst)) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
    }
}
