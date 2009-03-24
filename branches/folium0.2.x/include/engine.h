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
    int alpha_beta(int, int, int);
    int quiet(int, int);
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
};

inline int Engine::value()
{
    return (m_xq.player() == Red ? m_values[m_ply] : - m_values[m_ply]) + 4;//pawn value = 9
}


#endif //_ENGINE_H_