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
class XQ
{
public:
    XQ();
    XQ(string const &fen);
    XQ(XQ const &);
    XQ& operator =(XQ const &);
    operator string() const;
    uint square(uint) const;
    uint piece(uint) const;
    uint player() const;

    void make_null();
    void make_move(uint, uint);
    void unmake_null();
    void unmake_move(uint, uint, uint);

    bool is_legal_move(uint, uint) const;

    uint in_checked(uint) const;

    void generate_moves(MoveList&) const;
    void generate_capture_moves(MoveList&) const;
private:
    void clear();

    bool is_legal_king_move(uint src, uint dst) const;
    bool is_legal_advisor_move(uint src, uint dst) const;
    bool is_legal_bishop_move(uint src, uint dst) const;
    bool is_legal_rook_move(uint src, uint dst) const;
    bool is_legal_knight_move(uint src, uint dst) const;
    bool is_legal_cannon_move(uint src, uint dst) const;
    bool is_legal_red_pawn_move(uint src, uint dst) const;
    bool is_legal_black_pawn_move(uint src, uint dst) const;

    uint red_in_checked() const;
    uint black_in_checked() const;
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

inline uint XQ::square(uint idx) const
{
    assert (idx < 91UL);

    return m_squares[idx];
}
inline uint XQ::piece(uint idx) const
{
    assert (idx < 34UL);

    return m_pieces[idx];
}

inline uint32 XQ::player() const
{
    return m_player;
}

inline void XQ::make_null()
{
    m_player = 1UL - m_player;
}
inline void XQ::make_move(uint src, uint dst)
{
    uint32 src_piece = square(src);
    uint32 dst_piece = square(dst);
    assert (src == piece(src_piece));
    assert (dst_piece == EmptyIndex || dst == piece(dst_piece));
    assert (piece_color(src_piece) == m_player);
    assert (is_legal_move(src, dst));

    m_bitlines.changebit(square_x(src), square_y(src));
    m_bitlines.setbit(square_x(dst), square_y(dst));
    m_squares[src] = EmptyIndex;
    m_squares[dst] = src_piece;
    m_pieces[src_piece] = dst;
    m_pieces[dst_piece] = InvaildSquare;
    m_player = 1UL - m_player;
}
inline void XQ::unmake_null()
{
    m_player = 1UL - m_player;
}
inline void XQ::unmake_move(uint src, uint dst, uint dst_piece)
{
    uint32 src_piece = square(dst);
    assert (square(src) == EmptyIndex);
    assert (piece(src_piece) == dst);

    if (dst_piece == EmptyIndex)
    {
        m_bitlines.changebit(square_x(dst), square_y(dst));
    }
    m_bitlines.changebit(square_x(src), square_y(src));
    m_squares[src] = src_piece;
    m_squares[dst] = dst_piece;
    m_pieces[src_piece] = src;
    m_pieces[dst_piece] = dst;
    m_player = 1UL - m_player;

    assert (piece_color(src_piece) == m_player);
    assert (is_legal_move(src, dst));
}

inline uint XQ::in_checked(uint color) const
{
    return (color == Red ? red_in_checked() : black_in_checked()) ? 1 : 0;
}

inline bool XQ::is_legal_advisor_move(uint src, uint dst) const
{
    assert (piece_color(square(src)) != piece_color(square(dst)));
    assert (square_flag(src) & piece_flag(square(src)) & AdvisorFlag);

    if (!(square_flag(dst) & AdvisorFlag))
    {
        return false;
    }
    register uint x, y;
    x = square_x(src) - square_x(dst);
    y = square_y(src) - square_y(dst);
    return (x == 1UL || x == (0UL - 1UL)) && (y == 1UL || y == (0UL - 1UL));
}
inline bool XQ::is_legal_bishop_move(uint src, uint dst) const
{
    assert (piece_color(square(src)) != piece_color(square(dst)));
    assert (square_flag(src) & piece_flag(square(src)) & BishopFlag);

    return square(bishop_eye(src, dst)) == EmptyIndex;
}
inline bool XQ::is_legal_knight_move(uint src, uint dst) const
{
    assert (piece_color(square(src)) != piece_color(square(dst)));
    assert (square_flag(src) & piece_flag(square(src)) & KnightFlag);

    return square(knight_leg(src, dst)) == EmptyIndex;
}
inline bool XQ::is_legal_red_pawn_move(uint src, uint dst) const
{
    assert (piece_color(square(src)) != piece_color(square(dst)));
    assert (square_flag(src) & piece_flag(square(src)) & RedPawnFlag);
    return (square_flag(dst) & RedPawnFlag) && (dst == square_down(src) || dst == square_left(src) || dst == square_right(src));
}
inline bool XQ::is_legal_black_pawn_move(uint src, uint dst) const
{
    assert (piece_color(square(src)) != piece_color(square(dst)));
    assert (square_flag(src) & piece_flag(square(src)) & BlackPawnFlag);
    return (square_flag(dst) & BlackPawnFlag) && (dst == square_up(src) || dst == square_left(src) || dst == square_right(src));
}

#endif    //_XQ_H_
