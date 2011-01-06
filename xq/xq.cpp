#include <cstring>
#include <vector>
#include <map>
using namespace std;
#include "xq.h"

namespace folium
{
    XQ::XQ()
    {
        clear();
    }

    XQ::XQ(XQ const &xq)
    {
        m_bitmap = xq.m_bitmap;
        memcpy(m_pieces, xq.m_pieces, 34);
        memcpy(m_coordinates, xq.m_coordinates, 91);
        m_player = xq.m_player;
    }

    void XQ::clear()
    {
        m_bitmap.reset();
        memset(m_pieces, InvaildCoordinate, 34);
        memset(m_coordinates, EmptyIndex, 90);
        m_coordinates[90] = InvaildIndex;
        m_player = Empty;
    }

    static uint char_type(sint32 c)
    {
        static map<sint, uint> _map;
        if (_map.empty())
        {
            _map['K'] = RedKing;
            _map['G'] = RedAdvisor;
            _map['A'] = RedAdvisor;
            _map['B'] = RedBishop;
            _map['E'] = RedBishop;
            _map['R'] = RedRook;
            _map['H'] = RedKnight;
            _map['N'] = RedKnight;
            _map['C'] = RedCannon;
            _map['P'] = RedPawn;

            _map['k'] = BlackKing;
            _map['g'] = BlackAdvisor;
            _map['a'] = BlackAdvisor;
            _map['b'] = BlackBishop;
            _map['e'] = BlackBishop;
            _map['r'] = BlackRook;
            _map['h'] = BlackKnight;
            _map['n'] = BlackKnight;
            _map['c'] = BlackCannon;
            _map['p'] = BlackPawn;
        }
        if (_map.find(c) != _map.end())
            return _map[c];
        return InvaildPiece;
    }
    static sint type_char(uint32 t)
    {
        static map<uint, sint> _map;
        if (_map.empty())
        {
            _map[RedKing] = 'K';
            _map[RedAdvisor] = 'A';
            _map[RedBishop] = 'B';
            _map[RedRook] = 'R';
            _map[RedKnight] = 'N';
            _map[RedCannon] = 'C';
            _map[RedPawn] = 'P';

            _map[BlackKing] = 'k';
            _map[BlackAdvisor] = 'a';
            _map[BlackBishop] = 'b';
            _map[BlackRook] = 'r';
            _map[BlackKnight] = 'n';
            _map[BlackCannon] = 'c';
            _map[BlackPawn] = 'p';
        }
        if (_map.find(t) != _map.end())
            return _map[t];
        return 0;
    }
    static sint piece_char(uint piece)
    {
        return type_char(piece_type(piece));
    }

    bool XQ::set_fen(string const &fen)
    {
        clear();
        uint idx = 0UL, len = (uint)fen.size();
        uint y = 9UL, x = 0UL;
        while (idx < len)
        {
            sint32 c = fen[idx++];
            uint32 type = char_type(c);
            if (type != InvaildPiece)
            {
                if (y >= 10UL || x >= 9UL)
                {
                    //error
                    clear();
                    return false;
                }
                uint32 begin = type_begin(type);
                uint32 end = type_end(type);
                while (end >= begin)
                {
                    if (piece(begin) == InvaildCoordinate)
                    {
                        break;
                    }
                    ++begin;
                }
                if (begin > end)
                {
                    //error
                    clear();
                    return false;
                }
                uint32 sq = xy_coordinate(x++, y);
                m_coordinates[sq] = static_cast<uint8>(begin);
                m_pieces[begin] = static_cast<uint8>(sq);
            }
            else if (c == ' ')
            {
                if (y || x != 9UL || (idx == len))
                {
                    //error
                    clear();
                    return false;
                }
                c = fen[idx];
                m_player = ((c == 'b') ? Black : Red);
                for (int i = 0; i < 32; i++)
                {
                    uint32 sq = piece(i);
                    if (sq != InvaildCoordinate)
                    {
                        if (!(coordinate_flag(sq) & piece_flag(i)))
                        {
                            //error
                            clear();
                            return false;
                        }
                        m_bitmap.setbit(sq);
                    }
                }
                //ok
                return true;
            }
            else if (c == '/')
            {
                if (!y || x != 9UL)
                {
                    //error
                    clear();
                    return false;
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
                return false;
            }
        }
        //error
        clear();
        return false;
    }

    string XQ::get_fen()const
    {
        if (m_player == Empty)
            return string();
        vector<char> chars;
        for (sint y = 9; y >= 0; --y)
        {
            if (y != 9)
                chars.push_back('/');
            sint32 empty_count = 0;
            for (uint x = 0; x < 9; ++x)
            {
                uint sq = xy_coordinate(x,y);
                sint c = piece_char(coordinate(sq));
                if (c)
                {
                    if (empty_count)
                    {
                        chars.push_back(static_cast<char>('0' + empty_count));
                        empty_count = 0;
                    }
                    chars.push_back(static_cast<char>(c));
                }
                else
                {
                    ++empty_count;
                }
            }
            if (empty_count)
            {
                chars.push_back(static_cast<char>('0' + empty_count));
            }
        }
        chars.push_back(' ');
        chars.push_back(m_player == Red ? 'r' : 'b');
        return string(chars.begin(), chars.end());
    }

    bool XQ::do_move(uint src, uint dst)
    {
        uint32 src_piece = coordinate(src);
        uint32 dst_piece = coordinate(dst);
        assert (src == piece(src_piece));
        assert (dst_piece == EmptyIndex || dst == piece(dst_piece));
        assert (piece_color(src_piece) == m_player);

        m_bitmap.do_move(src, dst);
        m_coordinates[src] = static_cast<uint8>(EmptyIndex);
        m_coordinates[dst] = static_cast<uint8>(src_piece);
        m_pieces[src_piece] = static_cast<uint8>(dst);
        m_pieces[dst_piece] = static_cast<uint8>(InvaildCoordinate);
        uint incheck = 0;
        switch(src_piece)
        {
        case RedKingIndex:
        {
            incheck = ((coordinate_flag(coordinate_up(dst))
                     |coordinate_flag(coordinate_left(dst))
                     |coordinate_flag(coordinate_right(dst)))
                    & BlackPawnFlag);
        }
        case RedAdvisorIndex1:
        case RedAdvisorIndex2:
        case RedBishopIndex1:
        case RedBishopIndex2:
        case RedRookIndex1:
        case RedRookIndex2:
        case RedKnightIndex1:
        case RedKnightIndex2:
        case RedCannonIndex1:
        case RedCannonIndex2:
        case RedPawnIndex1:
        case RedPawnIndex2:
        case RedPawnIndex3:
        case RedPawnIndex4:
        case RedPawnIndex5:
        {
            uint kp = piece(RedKingIndex);
            assert(kp != InvaildCoordinate);
            incheck |= ((coordinate_flag(nonempty_down_1(kp))
                       | coordinate_flag(nonempty_up_1(kp))
                       | coordinate_flag(nonempty_left_1(kp))
                       | coordinate_flag(nonempty_right_1(kp)))
                      & (BlackRookFlag | BlackKingFlag))//rook
                   | ((coordinate_flag(nonempty_down_2(kp))
                       | coordinate_flag(nonempty_up_2(kp))
                       | coordinate_flag(nonempty_left_2(kp))
                       | coordinate_flag(nonempty_right_2(kp)))
                      & BlackCannonFlag)//cannon
                   | ((coordinate_flag(knight_leg(piece(BlackKnightIndex1), kp))
                       | coordinate_flag(knight_leg(piece(BlackKnightIndex2), kp)))
                      & EmptyFlag);//knight
            break;
        }
        case BlackKingIndex:
        {
            incheck = ((coordinate_flag(coordinate_down(dst))
                     | coordinate_flag(coordinate_left(dst))
                     | coordinate_flag(coordinate_right(dst)))
                    & RedPawnFlag);
        }
        case BlackAdvisorIndex1:
        case BlackAdvisorIndex2:
        case BlackBishopIndex1:
        case BlackBishopIndex2:
        case BlackRookIndex1:
        case BlackRookIndex2:
        case BlackKnightIndex1:
        case BlackKnightIndex2:
        case BlackCannonIndex1:
        case BlackCannonIndex2:
        case BlackPawnIndex1:
        case BlackPawnIndex2:
        case BlackPawnIndex3:
        case BlackPawnIndex4:
        case BlackPawnIndex5:
            uint kp = piece(BlackKingIndex);
            assert(kp != InvaildCoordinate);
            incheck |= ((coordinate_flag(nonempty_down_1(kp))
                       | coordinate_flag(nonempty_up_1(kp))
                       | coordinate_flag(nonempty_left_1(kp))
                       | coordinate_flag(nonempty_right_1(kp)))
                      & (RedRookFlag | RedKingFlag))//rook
                   | ((coordinate_flag(nonempty_down_2(kp))
                       | coordinate_flag(nonempty_up_2(kp))
                       | coordinate_flag(nonempty_left_2(kp))
                       | coordinate_flag(nonempty_right_2(kp)))
                      & RedCannonFlag)//cannon
                   | ((coordinate_flag(knight_leg(piece(RedKnightIndex1), kp))
                       | coordinate_flag(knight_leg(piece(RedKnightIndex2), kp)))
                      & EmptyFlag);//knight
            break;
        }
        if (incheck)
        {
            m_bitmap.undo_move(src, dst, dst_piece);
            m_coordinates[src] = static_cast<uint8>(src_piece);
            m_coordinates[dst] = static_cast<uint8>(dst_piece);
            m_pieces[src_piece] = static_cast<uint8>(src);
            m_pieces[dst_piece] = static_cast<uint8>(dst);
            return false;
        }
        m_player = 1UL - m_player;
        return true;
    }

}//namespace folium
