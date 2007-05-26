#ifndef _BITLINES_H_
#define _BITLINES_H_

#include <string.h>
#include "defines.h"
#include "xq_data.h"

extern const uint16 g_line_infos[10][1024];
extern const uint8 g_bit_counts[1024];
extern const uint16 g_distance_infos[91][128];

class BitLines
{
public:
    BitLines();
    void do_move(uint, uint);
    void undo_move(uint, uint, uint);

    uint square_up_1(uint)const;
    uint square_down_1(uint)const;
    uint square_left_1(uint)const;
    uint square_right_1(uint)const;
    uint square_up_2(uint)const;
    uint square_down_2(uint)const;
    uint square_left_2(uint)const;
    uint square_right_2(uint)const;

    uint distance(uint, uint)const;
    
    void reset ();
    void setbit(uint);
private:
    uint xinfo(uint, uint)const;
    uint yinfo(uint, uint)const;
    
    void changebit(uint);

    static uint prev_1(uint info);
    static uint prev_2(uint info);
    static uint next_1(uint info);
    static uint next_2(uint info);
private:
    uint16 m_lines[20];
};
//BitLines
inline uint BitLines::xinfo(uint x, uint y)const
{
    assert(x < 9UL && y < 10UL);
    return g_line_infos[x][m_lines[y]];
}
inline uint BitLines::yinfo(uint x, uint y)const
{
    assert(x < 9UL && y < 10UL);
    return g_line_infos[y][m_lines[x+10]];
}
inline void BitLines::setbit(uint sq)
{
    uint x = square_x(sq);
    uint y = square_y(sq);
    m_lines[y] |= 1UL << x;
    m_lines[x+10UL] |= 1UL << y;
}
inline void BitLines::changebit(uint sq)
{
    uint x = square_x(sq);
    uint y = square_y(sq);
    m_lines[y] ^= 1UL << x;
    m_lines[x+10UL] ^= 1UL << y;
}
inline uint BitLines::prev_1(uint info)
{
    return info & 0xf;
}
inline uint BitLines::prev_2(uint info)
{
    return (info >> 4) & 0xf;
}
inline uint32 BitLines::next_1(uint info)
{
    return (info >> 8) & 0xf;
}
inline uint BitLines::next_2(uint info)
{
    return (info >> 12) & 0xf;
}

inline void BitLines::do_move(uint src, uint dst)
{
    changebit(src);
    setbit(dst);
}
inline void BitLines::undo_move(uint src, uint dst, uint dst_piece)
{
    if (dst_piece == EmptyIndex)
        changebit(dst);
    changebit(src);
}

inline uint BitLines::square_up_1(uint sq)const
{
    uint x = square_x(sq);
    uint y = square_y(sq);
    return xy_square(x, BitLines::prev_1(yinfo(x, y)));
}
inline uint BitLines::square_down_1(uint sq)const
{
    uint x = square_x(sq);
    uint y = square_y(sq);
    return xy_square(x, BitLines::next_1(yinfo(x, y)));
}
inline uint BitLines::square_left_1(uint sq)const
{
    uint x = square_x(sq);
    uint y = square_y(sq);
    return xy_square(BitLines::prev_1(xinfo(x, y)), y);
}
inline uint BitLines::square_right_1(uint sq)const
{
    uint x = square_x(sq);
    uint y = square_y(sq);
    return xy_square(BitLines::next_1(xinfo(x, y)), y);
}
inline uint BitLines::square_up_2(uint sq)const
{
    uint x = square_x(sq);
    uint y = square_y(sq);
    return xy_square(x, BitLines::prev_2(yinfo(x, y)));
}
inline uint BitLines::square_down_2(uint sq)const
{
    uint x = square_x(sq);
    uint y = square_y(sq);
    return xy_square(x, BitLines::next_2(yinfo(x, y)));
}
inline uint BitLines::square_left_2(uint sq)const
{
    uint x = square_x(sq);
    uint y = square_y(sq);
    return xy_square(BitLines::prev_2(xinfo(x, y)), y);
}
inline uint BitLines::square_right_2(uint sq)const
{
    uint x = square_x(sq);
    uint y = square_y(sq);
    return xy_square(BitLines::next_2(xinfo(x, y)), y);
}

inline uint BitLines::distance(uint src, uint dst)const
{
    uint info = g_distance_infos[src][dst];
    assert((info >> 10) < 20);
    return g_bit_counts[info & 1023 & m_lines[info >> 10]];
}

inline BitLines::BitLines()
{
    reset();
}
inline void BitLines::reset()
{
    memset(m_lines, 0, 38);
    m_lines[20] = 1023;
}
#endif    //_BITLINES_H_

