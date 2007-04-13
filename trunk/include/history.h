#ifndef _HISTORY_H
#define _HISTORY_H

#include <cstring>

#include "defines.h"

extern const uint16 g_capture_scores[32][32];

class History
{
public:
    void clear();
    void update_history(uint32, uint);
    uint32 update_move(uint32)const;
    static uint32 update_capture_move(uint32, uint, uint);
private:
    uint32 m_scores[0x4000];
};

inline void History::clear()
{
    memset(m_scores, 0, 0x10000);
}

inline void History::update_history(uint32 move, uint depth)
{
    uint32 &score = m_scores[move & 0x3fff];
    score += depth * depth;
    if (score >= 262144)//262144 == 1 << 18
    {
        for(uint i = 0; i < 0x4000; ++i)
            m_scores[i] >>= 1;
    }
}

inline uint32 History::update_move(uint32 move)const
{
    return static_cast<uint32>(m_scores[move & 0x3fff] << 14) | (move & 0x3fff);
}

inline uint32 History::update_capture_move(uint32 move, uint src_piece, uint dst_piece)
{
    assert (src_piece < EmptyIndex);
    assert (dst_piece < EmptyIndex);
    return static_cast<uint32>(g_capture_scores[src_piece][dst_piece]) << 14 | (move & 0x3fff);
}

#endif  //_HISTORY_H
