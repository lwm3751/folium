#ifndef _BITLINES_H_
#define _BITLINES_H_

#include "defines.h"
#include "xq_data.h"

extern const uint16 g_line_infos[10][1024];

class BitLines
{
    class Line
    {
    public:
        operator uint()const;
        void reset(uint);
        uint getbit(uint)const;
        void setbit(uint);
        void clearbit(uint);
        void changebit(uint);
    private:
        uint16 m_content;
    };
public:
    BitLines();
    void do_move(uint, uint);
    void undo_move(uint, uint, uint);

    uint square_up_1(uint sq)const;
    uint square_down_1(uint sq)const;
    uint square_left_1(uint sq)const;
    uint square_right_1(uint sq)const;
    uint square_up_2(uint sq)const;
    uint square_down_2(uint sq)const;
    uint square_left_2(uint sq)const;
    uint square_right_2(uint sq)const;

    void reset ();
    void setbit(uint, uint);
private:
    uint xinfo(uint, uint)const;
    uint yinfo(uint, uint)const;
    
    void changebit(uint, uint);
    void clearbit(uint, uint);

    static uint prev_1(uint info);
    static uint prev_2(uint info);
    static uint next_1(uint info);
    static uint next_2(uint info);
private:
    Line m_xline[10];
    Line m_yline[9];
};
//BitLines::Line
inline BitLines::Line::operator uint()const
{
    return m_content;
}
inline void BitLines::Line::reset(uint32)
{
    m_content = 0;
}
inline uint BitLines::Line::getbit(uint pos)const
{
    return m_content & (1 << pos);
}
inline void BitLines::Line::setbit(uint pos)
{
    m_content |= (1 << pos);
}
inline void BitLines::Line::clearbit(uint pos)
{
    m_content &= ~(1 << pos);
}
inline void BitLines::Line::changebit(uint pos)
{
    m_content ^= (1 << pos);
}
//BitLines
inline uint BitLines::xinfo(uint x, uint y)const
{
    assert(x < 9UL && y < 10UL);
    return g_line_infos[x][m_xline[y]];
}
inline uint BitLines::yinfo(uint x, uint y)const
{
    assert(x < 9UL && y < 10UL);
    return g_line_infos[y][m_yline[x]];
}
inline void BitLines::setbit(uint x, uint y)
{
    assert(x < 9UL && y < 10UL);
    m_xline[y].setbit(x);
    m_yline[x].setbit(y);
}
inline void BitLines::changebit(uint x, uint y)
{
    assert(x < 9UL && y < 10UL);
    m_xline[y].changebit(x);
    m_yline[x].changebit(y);
}
inline void BitLines::clearbit(uint x, uint y)
{
    assert(x < 9UL && y < 10UL);
    m_xline[y].clearbit(x);
    m_yline[x].clearbit(y);
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
    changebit(square_x(src), square_y(src));
    setbit(square_x(dst), square_y(dst));
}
inline void BitLines::undo_move(uint src, uint dst, uint dst_piece)
{
    if (dst_piece == EmptyIndex)
        changebit(square_x(dst), square_y(dst));
    changebit(square_x(src), square_y(src));
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
#endif    //_BITLINES_H_

