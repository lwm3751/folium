#include "movelist.h"
#include "xq.h"
#include "xq_data.h"
#include "history.h"

void XQ::generate_moves(MoveList &ml) const
{
    uint own = player();
    uint opp = 1 - own;
    uint idx;
    uint src, dst;
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
            if (square_color(dst) != own)
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
                if (square_color(dst) != own)
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
            if (square_color(dst) != own)
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
                if (square_color(dst) != own)
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
            if (square_color(dst) != own)
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
            if (square_color(dst) != own && square((dst + src) >> 1) == EmptyIndex)
                ml.push(src, dst);
            dst = *pm++;
        }
    }
    //rook
    for(uint i = 0; i < 2; ++i)
    {
        uint dst;
        src = piece(++idx);
        if (src == InvaildSquare)
            continue;
        dst = nonempty_left_1(src);
        if (square_color(dst) == opp)
            ml.push(src, dst);
        for (uint tmp = dst, dst = square_left(src);
            dst != tmp;
            dst = square_left(dst))
            ml.push(src, dst);
        dst = nonempty_right_1(src);
        if (square_color(dst) == opp)
            ml.push(src, dst);
        for (uint tmp = dst, dst = square_right(src);
            dst != tmp;
            dst = square_right(dst))
            ml.push(src, dst);
        dst = nonempty_up_1(src);
        if (square_color(dst) == opp)
            ml.push(src, dst);
        for (uint tmp = dst, dst = square_up(src);
            dst != tmp;
            dst = square_up(dst))
            ml.push(src, dst);
        dst = nonempty_down_1(src);
        if (square_color(dst) == opp)
            ml.push(src, dst);
        for (uint tmp = dst, dst = square_down(src);
            dst != tmp;
            dst = square_down(dst))
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
            if (square(leg) == EmptyIndex && square_color(dst) != own)
                ml.push(src, dst);
            dst = *pm++;
        }
    }
    //cannon
    for(uint i = 0; i < 2; ++i)
    {
        uint dst;
        src = piece(++idx);
        if (src == InvaildSquare)
            continue;
        dst = nonempty_left_2(src);
        if (square_color(dst) == opp)
            ml.push(src, dst);
        for (uint tmp = nonempty_left_1(src), dst = square_left(src);
            dst != tmp;
            dst = square_left(dst))
            ml.push(src, dst);
        dst = nonempty_right_2(src);
        if (square_color(dst) == opp)
            ml.push(src, dst);
        for (uint tmp = nonempty_right_1(src), dst = square_right(src);
            dst != tmp;
            dst = square_right(dst))
            ml.push(src, dst);
        dst = nonempty_up_2(src);
        if (square_color(dst) == opp)
            ml.push(src, dst);
        for (uint tmp = nonempty_up_1(src), dst = square_up(src);
            dst != tmp;
            dst = square_up(dst))
            ml.push(src, dst);
        dst = nonempty_down_2(src);
        if (square_color(dst) == opp)
            ml.push(src, dst);
        for (uint tmp = nonempty_down_1(src), dst = square_down(src);
            dst != tmp;
            dst = square_down(dst))
            ml.push(src, dst);
    }
}

void XQ::generate_capture_moves(MoveList &ml) const
{
    uint own = player();
    uint opp = 1 - own;
    uint idx;
    uint src, dst;
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
            if (square_color(dst) == opp)
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
                if (square_color(dst) == opp)
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
            if (square_color(dst) == opp)
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
                if (square_color(dst) == opp)
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
            if (square_color(dst) == opp)
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
            if (square_color(dst) == opp && square((dst + src) >> 1) == EmptyIndex)
                ml.push(src, dst);
            dst = *pm++;
        }
    }
    //rook
    for(uint i = 0; i < 2; ++i)
    {
        uint dst;
        src = piece(++idx);
        if (src == InvaildSquare)
            continue;
        dst = nonempty_left_1(src);
        if (square_color(dst) == opp)
            ml.push(src, dst);
        dst = nonempty_right_1(src);
        if (square_color(dst) == opp)
            ml.push(src, dst);
        dst = nonempty_up_1(src);
        if (square_color(dst) == opp)
            ml.push(src, dst);
        dst = nonempty_down_1(src);
        if (square_color(dst) == opp)
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
            if (square(leg) == EmptyIndex && square_color(dst) == opp)
                ml.push(src, dst);
            dst = *pm++;
        }
    }
    //cannon
    for(uint i = 0; i < 2; ++i)
    {
        uint dst;
        src = piece(++idx);
        if (src == InvaildSquare)
            continue;
        dst = nonempty_left_2(src);
        if (square_color(dst) == opp)
            ml.push(src, dst);
        dst = nonempty_right_2(src);
        if (square_color(dst) == opp)
            ml.push(src, dst);
        dst = nonempty_up_2(src);
        if (square_color(dst) == opp)
            ml.push(src, dst);
        dst = nonempty_down_2(src);
        if (square_color(dst) == opp)
            ml.push(src, dst);
    }
}
void XQ::generate_moves(MoveList &ml, const History& history) const
{
    uint own = player();
    uint opp = 1 - own;
    uint idx;
    uint src, dst;
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
            if (square_color(dst) != own)
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
                if (square_color(dst) != own)
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
            if (square_color(dst) != own)
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
                if (square_color(dst) != own)
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
            if (square_color(dst) != own)
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
            if (square_color(dst) != own && square((dst + src) >> 1) == EmptyIndex)
                ml.push(history.move(src, dst, idx, square(dst)));
            dst = *pm++;
        }
    }
    //rook
    for(uint i = 0; i < 2; ++i)
    {
        uint dst;
        src = piece(++idx);
        if (src == InvaildSquare)
            continue;
        dst = nonempty_left_1(src);
        if (square_color(dst) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        for (uint tmp = dst, dst = square_left(src);
            dst != tmp;
            dst = square_left(dst))
            ml.push(history.move(src, dst, idx, EmptyIndex));
        dst = nonempty_right_1(src);
        if (square_color(dst) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        for (uint tmp = dst, dst = square_right(src);
            dst != tmp;
            dst = square_right(dst))
            ml.push(history.move(src, dst, idx, EmptyIndex));
        dst = nonempty_up_1(src);
        if (square_color(dst) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        for (uint tmp = dst, dst = square_up(src);
            dst != tmp;
            dst = square_up(dst))
            ml.push(history.move(src, dst, idx, EmptyIndex));
        dst = nonempty_down_1(src);
        if (square_color(dst) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        for (uint tmp = dst, dst = square_down(src);
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
            if (square(leg) == EmptyIndex && square_color(dst) != own)
                ml.push(history.move(src, dst, idx, square(dst)));
            dst = *pm++;
        }
    }
    //cannon
    for(uint i = 0; i < 2; ++i)
    {
        uint dst;
        src = piece(++idx);
        if (src == InvaildSquare)
            continue;
        dst = nonempty_left_2(src);
        if (square_color(dst) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        for (uint tmp = nonempty_left_1(src), dst = square_left(src);
            dst != tmp;
            dst = square_left(dst))
            ml.push(history.move(src, dst, idx, EmptyIndex));
        dst = nonempty_right_2(src);
        if (square_color(dst) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        for (uint tmp = nonempty_right_1(src), dst = square_right(src);
            dst != tmp;
            dst = square_right(dst))
            ml.push(history.move(src, dst, idx, EmptyIndex));
        dst = nonempty_up_2(src);
        if (square_color(dst) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        for (uint tmp = nonempty_up_1(src), dst = square_up(src);
            dst != tmp;
            dst = square_up(dst))
            ml.push(history.move(src, dst, idx, EmptyIndex));
        dst = nonempty_down_2(src);
        if (square_color(dst) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        for (uint tmp = nonempty_down_1(src), dst = square_down(src);
            dst != tmp;
            dst = square_down(dst))
            ml.push(history.move(src, dst, idx, EmptyIndex));
    }
}

void XQ::generate_capture_moves(MoveList &ml, const History& history) const
{
    uint own = player();
    uint opp = 1 - own;
    uint idx;
    uint src, dst;
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
            if (square_color(dst) == opp)
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
                if (square_color(dst) == opp)
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
            if (square_color(dst) == opp)
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
                if (square_color(dst) == opp)
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
            if (square_color(dst) == opp)
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
            if (square_color(dst) == opp && square((dst + src) >> 1) == EmptyIndex)
                ml.push(history.move(src, dst, idx, square(dst)));
            dst = *pm++;
        }
    }
    //rook
    for(uint i = 0; i < 2; ++i)
    {
        uint dst;
        src = piece(++idx);
        if (src == InvaildSquare)
            continue;
        dst = nonempty_left_1(src);
        if (square_color(dst) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        dst = nonempty_right_1(src);
        if (square_color(dst) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        dst = nonempty_up_1(src);
        if (square_color(dst) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        dst = nonempty_down_1(src);
        if (square_color(dst) == opp)
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
            if (square(leg) == EmptyIndex && square_color(dst) == opp)
                ml.push(history.move(src, dst, idx, square(dst)));
            dst = *pm++;
        }
    }
    //cannon
    for(uint i = 0; i < 2; ++i)
    {
        uint dst;
        src = piece(++idx);
        if (src == InvaildSquare)
            continue;
        dst = nonempty_left_2(src);
        if (square_color(dst) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        dst = nonempty_right_2(src);
        if (square_color(dst) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        dst = nonempty_up_2(src);
        if (square_color(dst) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
        dst = nonempty_down_2(src);
        if (square_color(dst) == opp)
            ml.push(history.move(src, dst, idx, square(dst)));
    }
}
