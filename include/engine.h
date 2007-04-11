#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "defines.h"
#include "move_list.h"
#include "xq_data.h"
#include "xq.h"
#include "history.h"
#include "hash.h"
#include "engine_data.h"

class Engine
{
public:
    Engine(const XQ&, uint hash);
    void reset(const XQ&);
    bool make_move(uint32 move);
    void unmake_move();
    uint32 search(int, MoveList&);
public:
    // for plugin
    void stop(){m_stop = true;}
    uint _square(uint s) const{return m_xq.square(s);}
    uint _piece(uint p) const{return m_xq.piece(p);}
    uint _player() const{return m_xq.player();}
    uint _in_checked() const{return m_xq.in_checked(m_xq.player());}
    uint _ply() const{return m_ply;}
private:
    void generate_root_move(MoveList& movelist, MoveList& ban);
    int value();
    int alpha_beta(int, int, int);
    int quiet(int, int);
    //int mini_window_quiet(int);
    //int full_window_quiet(int);
    //int mini_window_search(int, int);
    //int full_window_search(int, int, int);
    XQ m_xq;
    int m_ply;//current ply
    int m_start_ply;//start search ply
    int m_null_ply;//null move ply
    uint32 m_keys[512];
    uint64 m_locks[512];
    sint32 m_values[512];
    uint32 m_traces[512];
    History m_history;
    HashTable m_hash;
    bool m_stop;
private:
    uint m_tree_nodes;
    uint m_quiet_nodes;
    uint m_hash_hit_nodes;
    uint m_hash_move_cuts;
};

inline bool Engine::make_move(uint32 move)
{
    register uint src, dst, src_piece, dst_piece;
    register uint own;
    register uint flag;
    register int op, np;
    src = move_src(move);
    dst = move_dst(move);
    assert(m_xq.is_legal_move(src, dst));
    dst_piece = m_xq.square(dst);
    
    own = m_xq.player();
    m_xq.make_move(src, dst);
    if (m_xq.in_checked(own))
    {
        m_xq.unmake_move(src, dst, dst_piece);
        return false;
    }
    src_piece = m_xq.square(dst);
    flag = m_xq.in_checked(1 - own);
    op = m_ply;
    np = op + 1;

    m_traces[np] = create_trace(flag, dst_piece, move);
    if (dst_piece == EmptyIndex)
    {
        m_keys[np] = m_keys[op]\
                    ^ piece_key(src_piece, src)
                    ^ piece_key(src_piece, dst);
        m_locks[np] = m_locks[op]\
                    ^ piece_lock(src_piece, src)\
                    ^ piece_lock(src_piece, dst);;
        m_values[np] = m_values[op]\
                        + piece_value(src_piece, dst)\
                        - piece_value(src_piece, src);
    }
    else
    {
        m_keys[np] = m_keys[op]\
                    ^ piece_key(src_piece, src)\
                    ^ piece_key(src_piece, dst)\
                    ^ piece_key(dst_piece, dst);
        m_locks[np] = m_locks[op]\
                    ^ piece_lock(src_piece, src)\
                    ^ piece_lock(src_piece, dst)\
                    ^ piece_lock(dst_piece, dst);
        m_values[np] = m_values[op]\
                        + piece_value(src_piece, dst)\
                        - piece_value(src_piece, src)\
                        - piece_value(dst_piece, src);
    }
    m_ply = np;
    return true;
}
inline void Engine::unmake_move()
{
    assert (m_ply > 0);
    register uint src, dst, dst_piece;
    register uint32 trace = m_traces[m_ply--];
    src = trace_src(trace);
    dst = trace_dst(trace);
    dst_piece = trace_dst_piece(trace);
    m_xq.unmake_move(src, dst, dst_piece);
}

inline int Engine::value()
{
    return (m_xq.player() == Red ? m_values[m_ply] : - m_values[m_ply]) + 4;//pawn value = 9
}


#endif //_ENGINE_H_
