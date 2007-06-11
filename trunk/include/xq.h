#ifndef _XQ_H_
#define _XQ_H_

#include <vector>
using std::vector;
#include <string>
using std::string;

#include "defines.h"
#include "bitboard.h"
#include "xq_data.h"

#include <iostream>
using namespace std;

inline uint count32(uint32 v)
{
    v = ((v >> 1) & 0x55555555) + (v & 0x55555555);
    v = ((v >> 2) & 0x33333333) + (v & 0x33333333);
    v = ((v >> 4) & 0x0f0f0f0f) + (v & 0x0f0f0f0f);
    v = ((v >> 8) & 0x00ff00ff) + (v & 0x00ff00ff);
    return (v >> 16) + (v & 0x0000ffff);
}

class MoveList;
class History;
class XQ
{
public:
    XQ();
    XQ(string const &fen);
    XQ(XQ const &);
    XQ& operator =(XQ const &);
    operator string()const;
    uint square(uint)const;
    uint square_color(uint)const;
    uint piece(uint)const;
    uint player()const;

    void do_move(uint, uint);
    void undo_move(uint, uint, uint);
    void do_null();
    void undo_null();

    bool is_legal_move(uint, uint)const;
    bool is_legal_move(uint32)const;

    uint check_status()const;
    uint is_win()const;
    bool is_protected(uint pos, uint side)const;
    bool is_good_cap(uint move)const;

    vector<uint> generate_moves()const;
    void generate_moves(MoveList&, const History&)const;
    void generate_capture_moves(MoveList&, const History&)const;

    uint nonempty_up_1(uint)const;
    uint nonempty_down_1(uint)const;
    uint nonempty_left_1(uint)const;
    uint nonempty_right_1(uint)const;
    uint nonempty_up_2(uint)const;
    uint nonempty_down_2(uint)const;
    uint nonempty_left_2(uint)const;
    uint nonempty_right_2(uint)const;

private:
    void clear();
private:
    BitBoard m_bitboard;
    uint8 m_pieces[34];
    uint8 m_squares[91];
    uint8 m_player;
};
inline XQ::XQ()
{
    clear();
}
inline XQ::XQ(XQ const &xq)
{
    *this = xq;
}

inline uint XQ::square(uint idx)const
{
    assert (idx < 91UL);
    return m_squares[idx];
}
inline uint XQ::square_color(uint sq)const
{
    return piece_color(square(sq));
}
inline uint XQ::piece(uint idx)const
{
    assert (idx < 34UL);
    return m_pieces[idx];
}

inline uint32 XQ::player()const
{
    return m_player;
}

inline void XQ::do_null()
{
    m_player = 1 - m_player;
}
inline void XQ::undo_null()
{
    m_player = 1 - m_player;
}

inline bool XQ::is_legal_move(uint32 move)const
{
    return is_legal_move(move_src(move), move_dst(move));
}

inline uint XQ::nonempty_up_1(uint sq)const
{
    return m_bitboard.nonempty_up_1(sq);
}
inline uint XQ::nonempty_up_2(uint sq)const
{
    return m_bitboard.nonempty_up_2(sq);
}
inline uint XQ::nonempty_down_1(uint sq)const
{
    return m_bitboard.nonempty_down_1(sq);
}
inline uint XQ::nonempty_down_2(uint sq)const
{
    return m_bitboard.nonempty_down_2(sq);
}
inline uint XQ::nonempty_right_1(uint sq)const
{
    return m_bitboard.nonempty_right_1(sq);
}
inline uint XQ::nonempty_right_2(uint sq)const
{
    return m_bitboard.nonempty_right_2(sq);
}
inline uint XQ::nonempty_left_1(uint sq)const
{
    return m_bitboard.nonempty_left_1(sq);
}
inline uint XQ::nonempty_left_2(uint sq)const
{
    return m_bitboard.nonempty_left_2(sq);
}

#endif    //_XQ_H_
