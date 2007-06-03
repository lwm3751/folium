#include <string.h>
#include "xq.h"
/*大写表示红方，小写表示黑方*/
inline uint char_type(sint32 c)
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
inline sint type_char(uint32 type)
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
inline sint piece_char(uint piece)
{
    return type_char(piece_type(piece));
}

void XQ::clear()
{
    m_bitboard.reset();
    memset(m_pieces, InvaildSquare, 34);
    memset(m_squares, EmptyIndex, 90);
    m_squares[90] = InvaildIndex;
    m_player = Empty;
}
XQ::XQ(string const &fen)
{
    clear();
    uint idx = 0UL, len = fen.size();
    uint y = 9UL, x = 0UL;
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
                    m_bitboard.setbit(sq);
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
    m_bitboard = xq.m_bitboard;
    memcpy(m_pieces, xq.m_pieces, 34);
    memcpy(m_squares, xq.m_squares, 91);
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
    if (src > 90 || dst > 90)
        return false;
    uint32 src_piece = square(src);
    //这里同时排除了src == dst
    if (piece_color(src_piece) == piece_color(square(dst)))
        return false;
    if (!(g_move_flags[dst][src] & piece_flag(src_piece)))
        return false;
    switch (piece_type(src_piece))
    {
    case RedKing:
        if (g_move_flags[dst][src] & RedPawnFlag)
            return true;
        return (piece_flag(square(dst)) & BlackKingFlag) && m_bitboard.distance(src, dst) == 0;
    case BlackKing:
        if (g_move_flags[dst][src] & BlackPawnFlag)
            return true;
        return (piece_flag(square(dst)) & RedKingFlag) && m_bitboard.distance(src, dst) == 0;
    case RedAdvisor:
    case BlackAdvisor:
        return true;
    case RedBishop:
    case BlackBishop:
        return square(bishop_eye(src, dst)) == EmptyIndex;
    case RedRook:
    case BlackRook:
        return m_bitboard.distance(src, dst) == 0;
    case RedKnight:
    case BlackKnight:
        return square(knight_leg(src, dst)) == EmptyIndex;
    case RedCannon:
    case BlackCannon:
        return (m_bitboard.distance(src, dst) + (square(dst) >> 5)) == 1;
    case RedPawn:
    case BlackPawn:
        return true;
    default:
        return false;
    }
}
uint XQ::check_status()const
{
    const uint ksq = piece(m_player == Red ? RedKingIndex : BlackKingIndex);
    const uint pawn_flag = (m_player == Red ? BlackPawnFlag : RedPawnFlag);
    uint idx = (m_player == Red ? BlackRookIndex1 : RedRookIndex1);

    for (uint i = 0; i < 2; ++i)
    {
        uint sq = piece(idx);
        if (g_move_flags[ksq][sq] & RookFlag)
        {
            if (m_bitboard.distance(sq, ksq) == 0)
                return idx;
        }
        ++idx;
    }
    for (uint i = 0; i < 2; ++i)
    {
        uint sq = piece(idx);
        if (g_move_flags[ksq][sq] & KnightFlag)
        {
            if (square(knight_leg(sq, ksq)) == EmptyIndex)
                return 2;
        }
        ++idx;
    }
    for (uint i = 0; i < 2; ++i)
    {
        uint sq = piece(idx);
        if (g_move_flags[ksq][sq] & RookFlag)
        {
            if (m_bitboard.distance(sq, ksq) == 1)
                return 3;
        }
        ++idx;
    }
    for (uint i = 0; i < 5; ++i)
    {
        uint sq = piece(idx);
        if (g_move_flags[ksq][sq] & pawn_flag)
            return idx;
        ++idx;
    }
    return 0;
}
uint XQ::is_win()const
{
    uint flag;
    if (m_player == Black)
    {
        uint kp = piece(RedKingIndex);
        assert(kp != InvaildSquare);
        flag =
            //pawn
            ((piece_flag(square(square_down(kp)))
            | piece_flag(square(square_left(kp)))
            | piece_flag(square(square_right(kp))))
            & BlackPawnFlag)
            //rook
            | ((piece_flag(square(m_bitboard.nonempty_up_1(kp)))
            | piece_flag(square(m_bitboard.nonempty_down_1(kp)))
            | piece_flag(square(m_bitboard.nonempty_left_1(kp)))
            | piece_flag(square(m_bitboard.nonempty_right_1(kp))))
            & (BlackRookFlag | BlackKingFlag))
            //cannon
            | ((piece_flag(square(m_bitboard.nonempty_up_2(kp)))
            | piece_flag(square(m_bitboard.nonempty_down_2(kp)))
            | piece_flag(square(m_bitboard.nonempty_left_2(kp)))
            | piece_flag(square(m_bitboard.nonempty_right_2(kp))))
            & BlackCannonFlag)
            //knight
            | ((piece_flag(square(knight_leg(piece(BlackKnightIndex1), kp))) 
            | piece_flag(square(knight_leg(piece(BlackKnightIndex2), kp))))
            & EmptyFlag);
    }
    else
    {
        uint kp = piece(BlackKingIndex);
        assert(kp != InvaildSquare);
        flag =
            //pawn
            ((piece_flag(square(square_up(kp)))
            | piece_flag(square(square_left(kp)))
            | piece_flag(square(square_right(kp))))
            & RedPawnFlag)
            //rook
            | ((piece_flag(square(m_bitboard.nonempty_up_1(kp)))
            | piece_flag(square(m_bitboard.nonempty_down_1(kp)))
            | piece_flag(square(m_bitboard.nonempty_left_1(kp)))
            | piece_flag(square(m_bitboard.nonempty_right_1(kp))))
            & (RedRookFlag | RedKingFlag))
            //cannon
            | ((piece_flag(square(m_bitboard.nonempty_up_2(kp)))
            | piece_flag(square(m_bitboard.nonempty_down_2(kp)))
            | piece_flag(square(m_bitboard.nonempty_left_2(kp)))
            | piece_flag(square(m_bitboard.nonempty_right_2(kp))))
            & RedCannonFlag)
            //knight
            | ((piece_flag(square(knight_leg(piece(RedKnightIndex1), kp))) 
            | piece_flag(square(knight_leg(piece(RedKnightIndex2), kp))))
            & EmptyFlag);
    }
    return flag;
}

void XQ::make_move(uint src, uint dst)
{
    uint32 src_piece = square(src);
    uint32 dst_piece = square(dst);
    assert (src == piece(src_piece));
    assert (dst_piece == EmptyIndex || dst == piece(dst_piece));
    assert (piece_color(src_piece) == m_player);
    assert (is_legal_move(src, dst));

    m_bitboard.do_move(src, dst);
    m_squares[src] = EmptyIndex;
    m_squares[dst] = src_piece;
    m_pieces[src_piece] = dst;
    m_pieces[dst_piece] = InvaildSquare;
    m_player = 1UL - m_player;
}

void XQ::unmake_move(uint src, uint dst, uint dst_piece)
{
    uint32 src_piece = square(dst);
    assert (square(src) == EmptyIndex);
    assert (piece(src_piece) == dst);

    m_bitboard.undo_move(src, dst, dst_piece);
    m_squares[src] = src_piece;
    m_squares[dst] = dst_piece;
    m_pieces[src_piece] = src;
    m_pieces[dst_piece] = dst;
    m_player = 1UL - m_player;

    assert (piece_color(src_piece) == m_player);
    assert (is_legal_move(src, dst));
}
