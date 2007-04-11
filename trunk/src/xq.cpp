#include <algorithm>
using std::copy;
using std::fill;
#include "xq.h"
/*大写表示红方，小写表示黑方*/
inline uint32 char_type(sint32 c)
{
    switch(c)
    {
    case 'K':
        return RedKing;
    case 'k':
        return BlackKing;
    case 'G':
    case 'A':
        return RedAdvisor;
    case 'g':
    case 'a':
        return BlackAdvisor;
    case 'B':
    case 'E':
        return RedBishop;
    case 'b':
    case 'e':
        return BlackBishop;
    case 'R':
        return RedRook;
    case 'r':
        return BlackRook;
    case 'H':
    case 'N':
        return RedKnight;
    case 'h':
    case 'n':
        return BlackKnight;
    case 'C':
        return RedCannon;
    case 'c':
        return BlackCannon;
    case 'P':
        return RedPawn;
    case 'p':
        return BlackPawn;
    default:
        return InvaildPiece;
    }
}
inline sint32 type_char(uint32 type)
{
    switch(type)
    {
    case RedKing:
        return 'K';
    case BlackKing:
        return 'k';
    case RedAdvisor:
        return 'A';
    case BlackAdvisor:
        return 'a';
    case RedBishop:
        return 'B';
    case BlackBishop:
        return 'b';
    case RedRook:
        return 'R';
    case BlackRook:
        return 'r';
    case RedKnight:
        return 'N';
    case BlackKnight:
        return 'n';
    case RedCannon:
        return 'C';
    case BlackCannon:
        return 'c';
    case RedPawn:
        return 'P';
    case BlackPawn:
        return 'p';
    default:
        return 0;
    }
}
inline sint32 piece_char(uint32 piece)
{
    return type_char(piece_type(piece));
}

void XQ::clear()
{
    m_bitlines.reset();
    fill(m_pieces, m_pieces + 34, InvaildSquare);
    fill(m_squares, m_squares + 90, EmptyIndex);
    m_squares[90] = InvaildIndex;
    m_player = Empty;
}
XQ::XQ(string const &fen)
{
    clear();
    uint32 idx = 0UL, len = fen.size();
    uint32 y = 9UL, x = 0UL;
    while(idx < len)
    {
        sint32 c = fen[idx++];
        uint32 type = char_type(c);
        if (type != InvaildPiece)
        {
            if (y >= 10UL || x >= 9UL)
            {
                //error
                clear();
                return;
            }
            uint32 begin = type_begin(type);
            uint32 end = type_end(type);
            while (end >= begin)
            {
                if (piece(begin) == InvaildSquare)
                {
                    break;
                }
                ++begin;
            }
            if (begin > end)
            {
                //error
                clear();
                return;
            }
            uint32 sq = xy_square(x++, y);
            m_squares[sq] = begin;
            m_pieces[begin] = sq;
        }
        else if (c == ' ')
        {
            if (y || x != 9UL || (idx == len))
            {
                //error
                clear();
                return;
            }
            c = fen[idx];
            m_player = ((c == 'b') ? Black : Red);
            for (int i = 0; i < 32; i++)
            {
                uint32 sq = piece(i);
                if (sq != InvaildSquare)
                {
                    if (!(square_flag(sq) & piece_flag(i)))
                    {
                        //error
                        clear();
                        return;
                    }
                    m_bitlines.setbit(square_x(sq), square_y(sq));
                }
            }
            //ok
            return;
        }
        else if (c == '/')
        {
            if (!y || x != 9UL)
            {
                //error
                clear();
                return;
            }
            --y;
            x = 0UL;
        }
        else if (c >= '1' && c <= '9')
        {
            x += c - '0';
        }
        else
        {
            //error
            clear();
            return;
        }
    }
    //error
    clear();
}
XQ& XQ::operator=(const XQ &xq)
{
    m_bitlines = xq.m_bitlines;
    copy(xq.m_pieces, xq.m_pieces + 34, m_pieces);
    copy(xq.m_squares, xq.m_squares + 91, m_squares);
    m_player = xq.m_player;
    return *this;
}
XQ::operator string() const
{
    string fen;
    if (m_player == Empty)
        return fen;
    for (uint32 y = 9; y < 10; --y)
    {
        if(y != 9)
        {
            fen.push_back('/');
        }
        sint32 empty_count = 0;
        for (uint32 x = 0; x < 9; ++x)
        {
            uint32 sq = xy_square(x,y);
            sint32 c = piece_char(square(sq));
            if (c)
            {
                if (empty_count)
                {
                    fen.push_back('0' + empty_count);
                    empty_count = 0;
                }
                fen.push_back(c);
            }
            else
            {
                ++empty_count;
            }
        }
        if (empty_count)
        {
            fen.push_back('0' + empty_count);
        }
    }
    fen.append(m_player == Red ? " r - - 0 1" : " b - - 0 1");
    return fen;
}

bool XQ::is_legal_move(uint32 src, uint32 dst) const
{
    register uint32 src_piece = square(src);
    //这里同时排除了src == dst
    if (piece_color(src_piece) == piece_color(square(dst)))
    {
        return false;
    }
    switch (piece_type(src_piece))
    {
    case RedKing:
    case BlackKing:
        return is_legal_king_move(src, dst);
    case RedAdvisor:
    case BlackAdvisor:
        return is_legal_advisor_move(src, dst);
    case RedBishop:
    case BlackBishop:
        return is_legal_bishop_move(src, dst);
    case RedRook:
    case BlackRook:
        return is_legal_rook_move(src, dst);
    case RedKnight:
    case BlackKnight:
        return is_legal_knight_move(src, dst);
    case RedCannon:
    case BlackCannon:
        return is_legal_cannon_move(src, dst);
    case RedPawn:
        return is_legal_red_pawn_move(src, dst);
    case BlackPawn:
        return is_legal_black_pawn_move(src, dst);
    default:
        return false;
    }
}
bool XQ::is_legal_king_move(uint32 src, uint32 dst) const
{
    assert (piece_color(square(src)) != piece_color(square(dst)));
    assert (square_flag(src) & piece_flag(square(src)) & KingFlag);

    if (!(square_flag(dst) & KingFlag))
    {
        return false;
    }
    if (piece_flag(square(dst)) & KingFlag)
    {
        if (square_x(src) != square_x(dst))
        {
            return false;
        }
        for (uint32 f = piece(RedKingIndex) + 9, t = piece(BlackKingIndex); f != t; f += 9)
        {
            if (square(f) != EmptyIndex)
            {
                return false;
            }
        }
        return true;
    }
    return dst == square_up(src) || dst == square_down(src) || dst == square_left(src) || dst == square_right(src);
}
bool XQ::is_legal_rook_move(uint32 src, uint32 dst) const
{
    assert (piece_color(square(src)) != piece_color(square(dst)));
    assert (square_flag(src) & piece_flag(square(src)) & RookFlag);
    sint32 inc;
    {
        register uint32 sx, dx, sy, dy;
        sx = square_x(src);
        dx = square_x(dst);
        sy = square_y(src);
        dy = square_y(dst);
        if (sx == dx)
        {
            inc = sy > dy ? -9 : 9;
        }
        else if(sy == dy)
        {
            inc = sx > dx ? -1 : 1;
        }
        else
        {
            return false;
        }
    }
    for (uint32 target = src + inc; target != dst; target += inc)
    {
        if (square(target) != EmptyIndex)
        {
            return false;
        }
    }
    return true;
}
bool XQ::is_legal_cannon_move(uint32 src, uint32 dst) const
{
    assert (piece_color(square(src)) != piece_color(square(dst)));
    assert (square_flag(src) & piece_flag(square(src)) & CannonFlag);
    sint32 inc;
    {
        register uint32 sx, dx, sy, dy;
        sx = square_x(src);
        dx = square_x(dst);
        sy = square_y(src);
        dy = square_y(dst);
        if (sx == dx)
        {
            inc = sy > dy ? -9 : 9;
        }
        else if(sy == dy)
        {
            inc = sx > dx ? -1 : 1;
        }
        else
        {
            return false;
        }
    }
    register uint32 count = 0;
    for (uint32 target = src + inc; target != dst; target += inc)
    {
        if (square(target) != EmptyIndex)
        {
            ++count;
        }
    }
    return count == (square(dst) == EmptyIndex ? 0 : 1);
}

uint XQ::red_in_checked() const
{
    uint kp = piece(RedKingIndex);
    uint kx = square_x(kp);
    uint ky = square_y(kp);
    uint pf = (piece_flag(square(square_down(kp)))
                | piece_flag(square(square_left(kp)))
                | piece_flag(square(square_right(kp)))) & BlackPawnFlag;
    if (kp == 90)
        cout << string(*this) << endl;
    //cout << 2 << kp << "~~~" << kx << "~~~" << ky << endl;
    uint32 xinfo = m_bitlines.xinfo(kx, ky);
    uint rf = piece_flag(square(xy_square(LineInfo::get_prev_1(xinfo), ky)))
                | piece_flag(square(xy_square(LineInfo::get_next_1(xinfo), ky)));
    uint cf = piece_flag(square(xy_square(LineInfo::get_prev_2(xinfo), ky)))
                | piece_flag(square(xy_square(LineInfo::get_next_2(xinfo), ky)));
    uint32 yinfo = m_bitlines.yinfo(kx, ky);
    rf |= piece_flag(square(xy_square(kx, LineInfo::get_prev_1(yinfo))))
        | piece_flag(square(xy_square(kx, LineInfo::get_next_1(yinfo))));
    cf |= piece_flag(square(xy_square(kx, LineInfo::get_prev_2(yinfo))))
        | piece_flag(square(xy_square(kx, LineInfo::get_next_2(yinfo))));
    rf &= BlackRookFlag | BlackKingFlag;
    cf &= BlackCannonFlag;
    uint nf = (piece_flag(square(knight_leg(piece(BlackKnightIndex1), kp))) | piece_flag(square(knight_leg(piece(BlackKnightIndex2), kp)))) & EmptyFlag;
    return pf | rf | cf | nf;
}
uint XQ::black_in_checked() const
{
    uint kp = piece(BlackKingIndex);
    uint kx = square_x(kp);
    uint ky = square_y(kp);
    uint pf = (piece_flag(square(square_up(kp)))
                | piece_flag(square(square_left(kp)))
                | piece_flag(square(square_right(kp)))) & RedPawnFlag;
    if (kp == 90)
        cout << string(*this) << endl;
    //cout << 1 << kp << "~~~" << kx << "~~~" << ky << endl;
    const uint32 &xinfo = m_bitlines.xinfo(kx, ky);
    uint rf = piece_flag(square(xy_square(LineInfo::get_prev_1(xinfo), ky)))
                | piece_flag(square(xy_square(LineInfo::get_next_1(xinfo), ky)));
    uint cf = piece_flag(square(xy_square(LineInfo::get_prev_2(xinfo), ky)))
                | piece_flag(square(xy_square(LineInfo::get_next_2(xinfo), ky)));
    const uint32 &yinfo = m_bitlines.yinfo(kx, ky);
    rf |= piece_flag(square(xy_square(kx, LineInfo::get_prev_1(yinfo))))
        | piece_flag(square(xy_square(kx, LineInfo::get_next_1(yinfo))));
    cf |= piece_flag(square(xy_square(kx, LineInfo::get_prev_2(yinfo))))
        | piece_flag(square(xy_square(kx, LineInfo::get_next_2(yinfo))));
    rf &= RedRookFlag | RedKingFlag;
    cf &= RedCannonFlag;
    uint nf = (piece_flag(square(knight_leg(piece(RedKnightIndex1), kp))) | piece_flag(square(knight_leg(piece(RedKnightIndex2), kp)))) & EmptyFlag;
    return pf | rf | cf | nf;
}
