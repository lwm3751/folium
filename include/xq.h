#ifndef _XQ_H_
#define _XQ_H_

#include <string>
using std::string;

#include "defines.h"
#include "bitlines.h"
#include "xq_data.h"

#include <iostream>
using namespace std;

class MoveList;
class Engine;
class XQ
{
friend class Engine;
public:
    XQ();
    XQ(string const &fen);
    XQ(XQ const &);
    XQ& operator =(XQ const &);
    operator string()const;
    uint square(uint)const;
    uint piece(uint)const;
    uint player()const;

    void make_move(uint, uint);
    void unmake_move(uint, uint, uint);

    bool is_legal_move(uint, uint)const;
    bool is_legal_move(uint32)const;

    uint in_checked(uint)const;

    void generate_moves(MoveList&)const;
    void generate_capture_moves(MoveList&)const;
private:
    void clear();
    uint red_in_checked()const;
    uint black_in_checked()const;
private:
    BitLines m_bitlines;
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
inline uint XQ::piece(uint idx)const
{
    assert (idx < 34UL);
    return m_pieces[idx];
}

inline uint32 XQ::player()const
{
    return m_player;
}

inline bool XQ::is_legal_move(uint32 move)const
{
    return is_legal_move(move_src(move), move_dst(move));
}

inline uint XQ::in_checked(uint color)const
{
    return (color == Red ? red_in_checked() : black_in_checked()) ? 1 : 0;
}
#endif    //_XQ_H_
