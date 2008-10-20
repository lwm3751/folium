#ifndef _XQ_H_
#define _XQ_H_

#include <string>

#include "defines.h"
#include "move.h"
#include "bitmap.h"
#include "xq_data.h"

namespace folium
{
    using std::string;

    class XQ
    {
    public:
        XQ();
        XQ(XQ const &);

        string get_fen()const;
        bool set_fen(const string&);

        uint32 key()const;
        uint64 lock()const;
        sint32 value()const;

        uint square(uint)const;
        uint square_color(uint)const;
        uint square_flag(uint)const;
        uint piece(uint)const;
        uint player()const;

        bool do_move(uint, uint);
        void undo_move(uint, uint, uint);
        void do_null();
        void undo_null();

        bool is_legal_move(uint, uint)const;
        bool is_legal_move(uint32)const;

        bool is_good_cap(uint move)const;
        uint status()const;
        uint player_in_check(uint player) const;

        uint nonempty_up_1(uint)const;
        uint nonempty_down_1(uint)const;
        uint nonempty_left_1(uint)const;
        uint nonempty_right_1(uint)const;
        uint nonempty_down_2(uint)const;
        uint nonempty_up_2(uint)const;
        uint nonempty_left_2(uint)const;
        uint nonempty_right_2(uint)const;
        uint distance(uint, uint)const;
        uint distance_is_0(uint, uint)const;
        uint distance_is_1(uint, uint)const;
    private:
        void clear();
    private:
        Bitmap m_bitmap;
        uint8 m_pieces[34];
        uint8 m_squares[91];
        uint8 m_player;
    };

    inline uint XQ::square(uint idx)const
    {
        assert (idx < 91UL);
        return m_squares[idx];
    }
    inline uint XQ::square_color(uint sq)const
    {
        return piece_color(square(sq));
    }
    inline uint XQ::square_flag(uint sq)const
    {
        return piece_flag(square(sq));
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
        return m_bitmap.nonempty_up_1(sq);
    }
    inline uint XQ::nonempty_up_2(uint sq)const
    {
        return m_bitmap.nonempty_up_2(sq);
    }
    inline uint XQ::nonempty_down_1(uint sq)const
    {
        return m_bitmap.nonempty_down_1(sq);
    }
    inline uint XQ::nonempty_down_2(uint sq)const
    {
        return m_bitmap.nonempty_down_2(sq);
    }
    inline uint XQ::nonempty_right_1(uint sq)const
    {
        return m_bitmap.nonempty_right_1(sq);
    }
    inline uint XQ::nonempty_right_2(uint sq)const
    {
        return m_bitmap.nonempty_right_2(sq);
    }
    inline uint XQ::nonempty_left_1(uint sq)const
    {
        return m_bitmap.nonempty_left_1(sq);
    }
    inline uint XQ::nonempty_left_2(uint sq)const
    {
        return m_bitmap.nonempty_left_2(sq);
    }
    inline uint XQ::distance(uint src, uint dst)const
    {
        return m_bitmap.distance(src, dst);
    }
    inline uint XQ::distance_is_0(uint src, uint dst)const
    {
        return m_bitmap.distance_is_0(src, dst);
    }
    inline uint XQ::distance_is_1(uint src, uint dst)const
    {
        return m_bitmap.distance_is_1(src, dst);
    }

    inline bool XQ::do_move(uint src, uint dst)
    {
        uint32 src_piece = square(src);
        uint32 dst_piece = square(dst);
        assert (src == piece(src_piece));
        assert (dst_piece == EmptyIndex || dst == piece(dst_piece));
        assert (piece_color(src_piece) == m_player);
        assert (is_legal_move(src, dst));

        m_bitmap.do_move(src, dst);
        m_squares[src] = EmptyIndex;
        m_squares[dst] = src_piece;
        m_pieces[src_piece] = dst;
        m_pieces[dst_piece] = InvaildSquare;
        if (player_in_check(m_player))
        {
            m_bitmap.undo_move(src, dst, dst_piece);
            m_squares[src] = src_piece;
            m_squares[dst] = dst_piece;
            m_pieces[src_piece] = src;
            m_pieces[dst_piece] = dst;
            return false;
        }

        m_player = 1UL - m_player;
        return true;
    }
    inline void XQ::undo_move(uint src, uint dst, uint dst_piece)
    {
        uint32 src_piece = square(dst);
        assert (square(src) == EmptyIndex);
        assert (piece(src_piece) == dst);

        m_bitmap.undo_move(src, dst, dst_piece);
        m_squares[src] = src_piece;
        m_squares[dst] = dst_piece;
        m_pieces[src_piece] = src;
        m_pieces[dst_piece] = dst;
        m_player = 1UL - m_player;

        assert (piece_color(src_piece) == m_player);
        assert (is_legal_move(src, dst));
    }

}//namespace folium

#endif    //_XQ_H_
