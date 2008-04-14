#ifndef _GENERATOR_H_
#define _GENERATOR_H_

#include "defines.h"
#include "movelist.h"
#include "xq.h"
#include "killer.h"
#include "history.h"

class Generator
{
public:
    Generator(XQ& xq, uint32 hash_move, Killer& killer, History& history):
    m_xq(xq), m_hash_move(hash_move), m_killer(killer), m_history(history), m_stage(0), m_index(0)
    {
    }
    uint32 next();
private:
    XQ& m_xq;
    uint32 m_hash_move;
    Killer& m_killer;
    History& m_history;
    MoveList m_ml;
    int m_stage;
    int m_index;
};
#endif    //_GENERATOR_H_
