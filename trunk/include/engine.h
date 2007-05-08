#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "defines.h"
#include "movelist.h"
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
    void make_null();
    void unmake_null();
    uint32 search(int, MoveList&);
public:
    // for plugin
    void stop(){m_stop = true;}
    uint _square(uint s)const{return m_xq.square(s);}
    uint _piece(uint p)const{return m_xq.piece(p);}
    uint _player()const{return m_xq.player();}
    uint _in_checked()const{return m_xq.in_checked(m_xq.player());}
    uint _ply()const{return m_ply;}
private:
    void generate_root_move(MoveList& movelist, MoveList& ban);
    int value();
    int full(int, int, int);
    int mini(int, int);
    int quies(int, int);
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
    volatile bool m_stop;
private:
    uint m_tree_nodes;
    uint m_quiet_nodes;
    uint m_hash_hit_nodes;
    uint m_hash_move_cuts;
    uint m_null_nodes;
    uint m_null_cuts;
};

inline int Engine::value()
{
    return (m_xq.player() == Red ? m_values[m_ply] : - m_values[m_ply]) + 4;//pawn value = 9
}

inline void Engine::make_null()
{
    assert(!trace_flag(m_traces[m_ply]));
    assert(m_null_ply == m_start_ply);
    register uint op, np;
    op = m_ply;
    np = m_ply + 1;
    m_keys[np] = m_keys[op];
    m_locks[np] = m_locks[op];
    m_values[np] = m_values[op];
    m_traces[np] = 0;
    m_null_ply = m_ply = np;
    m_xq.m_player = 1UL - m_xq.m_player;
}

inline void Engine::unmake_null()
{
    --m_ply;
    m_null_ply = m_start_ply;
    m_xq.m_player = 1UL - m_xq.m_player;
}

#endif //_ENGINE_H_
