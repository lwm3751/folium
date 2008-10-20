#include <vector>
#include "xq.h"

namespace folium
{
    using namespace std;
    inline uint char_type(sint32 c)
    {
        switch (c)
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
        switch (type)
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
                    return false;
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
                    return false;
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
                uint sq = xy_square(x,y);
                sint c = piece_char(square(sq));
                if (c)
                {
                    if (empty_count)
                    {
                        chars.push_back('0' + empty_count);
                        empty_count = 0;
                    }
                    chars.push_back(c);
                }
                else
                {
                    ++empty_count;
                }
            }
            if (empty_count)
            {
                chars.push_back('0' + empty_count);
            }
        }
        chars.push_back(' ');
        chars.push_back(m_player == Red ? 'r' : 'b');
        return string(chars.begin(), chars.end());
    }
}
