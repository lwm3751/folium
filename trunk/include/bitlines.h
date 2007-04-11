#ifndef _BITLINES_H_
#define _BITLINES_H_

#include "defines.h"

extern const uint32 g_line_infos[10][1024];

class Line
{
public:
    operator uint() const;
    void reset(uint value=0);
    uint getbit(uint) const;
    void setbit(uint);
    void clearbit(uint);
    void changebit(uint);
private:
    uint16 m_content;
};
inline Line::operator uint()const
{
    return m_content;
}
inline void Line::reset(uint32 value /* = 0 */)
{
    m_content = value;
}
inline uint Line::getbit(uint pos)const
{
    return m_content & (1 << pos);
}
inline void Line::setbit(uint pos)
{
    m_content |= (1 << pos);
}
inline void Line::clearbit(uint pos)
{
    m_content &= ~(1 << pos);
}
inline void Line::changebit(uint pos)
{
    m_content ^= (1 << pos);
}

class LineInfo
{
public:
    static uint get_prev_1(const uint32 &info);
    static uint get_prev_2(const uint32 &info);
    static uint get_next_1(const uint32 &info);
    static uint get_next_2(const uint32 &info);
};

inline uint LineInfo::get_prev_1(const uint32 &info)
{
    return info & 0xff;
}
inline uint LineInfo::get_prev_2(const uint32 &info)
{
    return (info >> 8) & 0xff;
}
inline uint32 LineInfo::get_next_1(const uint32 &info)
{
    return (info >> 16) & 0xff;
}
inline uint LineInfo::get_next_2(const uint32 &info)
{
    return (info >> 24) & 0xff;
}

class BitLines
{
public:
    BitLines();
    BitLines(const BitLines&);

    const uint32& xinfo(uint, uint) const;
    const uint32& yinfo(uint, uint) const;

    void setbit(uint, uint);
    void changebit(uint, uint);
    void clearbit(uint, uint);

    void reset ();
private:
    Line m_xline[10];
    Line m_yline[9];
};
inline const uint32& BitLines::xinfo(uint x, uint y) const
{
    assert(x < 9UL && y < 10UL);
    return g_line_infos[x][m_xline[y]];
}
inline const uint32& BitLines::yinfo(uint x, uint y) const
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
#endif    //_BITLINES_H_

