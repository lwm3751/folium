#include "engine.h"
#include <iostream>
using namespace std;

const int LIMIT_DEPTH = 64;
const int NULL_DEPTH = 2;

class Killer
{
public:
    void clear();
    void update(MoveList&)const;
    void push(uint);
private:
    uint16 moves[2];
};
inline void Killer::clear()
{
    moves[0] = moves[1] = 0;
}
inline void Killer::update(MoveList& ml)const
{
    ml.push(moves[0]);
    ml.push(moves[1]);
}
inline void Killer::push(uint move)
{
    assert(move < 0x4000);
    if (move==moves[0])
        return;
    moves[1] = moves[0];
    moves[0] = move;
}

static Killer killers[LIMIT_DEPTH+1];
static MoveList movelists[LIMIT_DEPTH];
int Engine::full(int depth, int alpha, int beta)
{
    if (m_stop)
        return - WINSCORE;
    if (depth <= 0)
    {
        ++m_leaf_nodes;
        return quies(alpha, beta);
    }
    const int ply = m_ply - m_start_ply;
    const int flag = trace_flag(m_traces[m_ply]);

    ++m_tree_nodes;

    //—≠ª∑ÃΩ≤‚
    {
        int score = loop_value(ply);
        if (score != INVAILDVALUE)
            return score;
    }

    int best_value = ply - WINSCORE;

    //…±∆ÂºÙ≤√
    if (best_value >= beta)
        return best_value;

    if (ply == LIMIT_DEPTH)
        return value();

    uint32 best_move=0;
    Killer& killer=killers[ply];
    killers[ply+1].clear();
    int score;
    MoveList ml;

    int call_depth = depth - 1;
    //Ω´æ¸…Ï—”
    if (flag)
        ++call_depth;

    bool found = false;
    //ÃΩ≤‚hashmove “‘º∞killermove
    //*
    uint32 hash_move;
    m_hash.probe(call_depth, ply, alpha, beta, hash_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
    ml.push(hash_move);
    killer.update(ml);
    for (uint i=0; i<ml.size(); ++i)
    {
        uint32 move = ml[i];
        if (m_xq.player() != piece_color(m_xq.square(move_src(move))))
            continue;
        if (!m_xq.is_legal_move(move))
            continue;
        if (!make_move(move))
            continue;
        if (found)
        {
            score = - mini(call_depth, -alpha);
            if ((score > alpha) && (score < beta))
                score = - full(call_depth, -beta, -alpha);
        }
        else
            score = - full(call_depth, -beta, -alpha);
        unmake_move();
        if (score > best_value)
        {
            best_value = score;
            best_move = move & 0x3fff;
            if (score >= beta)
            {
                if (m_stop)
                    return - WINSCORE;
                if (i==0)
                    m_hash_move_cuts++;
                else if (i==1)
                    m_kill_cuts_1++;
                else
                    m_kill_cuts_2++;
                m_history.update_history(best_move, call_depth);
                m_hash.store_beta(call_depth, ply, score, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
                killer.push(best_move);
                return score;
            }
            if (score > alpha)
            {
                found = true;
                alpha = score;
            }
        }
    }//*/
    m_xq.generate_moves(ml, m_history);
    const uint size = ml.size();
    for (uint i = 0; i < size; ++i)
    {
        uint32 move = ml[i];
        for (uint j = i + 1; j < size; ++j)
        {
            if (ml[j] > move)
            {
                ml[i] = ml[j];
                ml[j] = move;
                move = ml[i];
            }
        }

        if (!make_move(move))
            continue;
        if (found)
        {
            score = - mini(call_depth, -alpha);
            if ((score > alpha) && (score < beta))
                score = - full(call_depth, -beta, -alpha);
        }
        else
            score = - full(call_depth, -beta, -alpha);
        unmake_move();

        if (score > best_value)
        {
            best_value = score;
            best_move = move & 0x3fff;
            if (score >= beta)
            {
                if (m_stop)
                    return - WINSCORE;
                m_history.update_history(best_move, call_depth);
                m_hash.store_beta(call_depth, ply, score, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
                killer.push(best_move);
                return score;
            }
            if (score > alpha)
            {
                found = true;
                alpha = score;
            }
        }
    }
    if (m_stop)
        return - WINSCORE;
    if (best_move)
    {
        killer.push(best_move);
        m_history.update_history(best_move, call_depth);
        if (!found)
            m_hash.store_alpha(call_depth, ply, best_value, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
        else
            m_hash.store_pv(call_depth, ply, best_value, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
    }
    return best_value;
}
int Engine::mini(int depth, int beta, bool do_null)
{
    if (depth <= 0)
    {
        ++m_leaf_nodes;
        return quies(beta-1, beta);
    }
    const int ply = m_ply - m_start_ply;
    const int flag = trace_flag(m_traces[m_ply]);

    //—≠ª∑ÃΩ≤‚
    {
        int score = loop_value(ply);
        if (score != INVAILDVALUE)
            return score;
    }

    int best_value = ply - WINSCORE;
    //…±∆ÂºÙ≤√
    if (best_value >= beta)
        return best_value;

    if (ply == LIMIT_DEPTH)
        return value();

    Killer& killer=killers[ply];
    killers[ply+1].clear();

    ++m_tree_nodes;
    uint32 best_move=0;
    int score;
    MoveList ml;

    int call_depth = depth - 1;
    //Ω´æ¸…Ï—”
    if (flag)
        ++call_depth;

    //hashÃΩ≤‚
    uint32 hash_move;
    score = m_hash.probe(call_depth, ply, beta-1, beta, hash_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
    if (m_xq.is_legal_move(hash_move) && m_xq.player() == piece_color(m_xq.square(move_src(hash_move))))
    {
        if (score != INVAILDVALUE)
        {
            m_hash_hit_nodes++;
            return score;
        }
    }

    //nullºÙ≤√
    if (call_depth >= 2 && flag == 0 && do_null && (value() - beta > 4) && beta < MATEVALUE && beta > -MATEVALUE)
    {
        ++m_null_nodes;
        make_null();
        score = - mini(call_depth - NULL_DEPTH, -beta+1, false);
        unmake_null();
        if (score >= beta)
        {
            if (mini(call_depth > NULL_DEPTH ? (call_depth - NULL_DEPTH) : 1, -beta+1, false) >= beta)
            {
                ++m_null_cuts;
                return score;
            }
        }
    }

    //ÃΩ≤‚hashmove “‘º∞killermove
    //*
    ml.push(hash_move);
    killer.update(ml);
    for (uint i=0; i<ml.size(); ++i)
    {
        uint32 move = ml[i];
        if (m_xq.player() != piece_color(m_xq.square(move_src(move))))
            continue;
        if (!m_xq.is_legal_move(move))
            continue;
        if (!make_move(move))
            continue;
        score = - mini(call_depth, -beta+1);
        unmake_move();
        if (score > best_value)
        {
            best_value = score;
            best_move = move & 0x3fff;
            if (score >= beta)
            {
                if (m_stop)
                    return - WINSCORE;
                if (i==0)
                    m_hash_move_cuts++;
                else if (i==1)
                    m_kill_cuts_1++;
                else
                    m_kill_cuts_2++;
                m_history.update_history(best_move, call_depth);
                m_hash.store_beta(call_depth, ply, score, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
                killer.push(best_move);
                return score;
            }
        }
    }//*/
    m_xq.generate_moves(ml, m_history);
    const uint size = ml.size();
    for (uint i = 0; i < size; ++i)
    {
        uint32 move = ml[i];
        for (uint j = i + 1; j < size; ++j)
        {
            if (ml[j] > move)
            {
                ml[i] = ml[j];
                ml[j] = move;
                move = ml[i];
            }
        }

        if (!make_move(move))
            continue;
        score = - mini(call_depth, -beta+1);
        unmake_move();

        if (m_stop)
            return - WINSCORE;

        if (score > best_value)
        {
            best_value = score;
            best_move = move & 0x3fff;
            if (best_value >= beta)
            {
                if (m_stop)
                    return -WINSCORE;
                m_history.update_history(best_move, call_depth);
                m_hash.store_beta(call_depth, ply, best_value, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
                killer.push(best_move);
                return beta;
            }
        }
    }
    if (m_stop)
        return - WINSCORE;
    if (best_move)
    {
        killer.push(best_move);
        m_history.update_history(best_move, call_depth);
        m_hash.store_alpha(call_depth, ply, best_value, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
    }
    return best_value;
}

int Engine::quies(int alpha, int beta)
{
    const int ply = m_ply - m_start_ply;
    const int flag = trace_flag(m_traces[m_ply]);

    m_quiet_nodes++;
    //—≠ª∑ÃΩ≤‚
    {
        int score = loop_value(ply);
        if (score != INVAILDVALUE)
            return score;
    }

    int score, best_value;
    best_value = ply - WINSCORE;
    if (best_value >= beta)
        return best_value;

    if (ply == LIMIT_DEPTH)
        return value();

    if (!flag)
    {
        score = value();
        if (score >= beta)
            return score;
        if (score > best_value)
        {
            best_value = score;
            if (score > alpha)
                alpha = score;
        }
    }
    MoveList ml;
    if (flag)
        m_xq.generate_moves(ml, m_history);
    else
        m_xq.generate_capture_moves(ml, m_history);
    uint size = ml.size();
    for (uint i = 0; i < size; ++i)
    {
        uint32 move = ml[i];
        for (uint j = i + 1; j < size; ++j)
        {
            if (ml[j] > move)
            {
                ml[i] = ml[j];
                ml[j] = move;
                move = ml[i];
            }
        }
        if (!make_move(move))
            continue;
        score = - quies(-beta, -alpha);
        unmake_move();

        if (score >= beta)
            return beta;
        if (score > best_value)
        {
            best_value = score;
            if (score > alpha)
                alpha = score;
        }
    }
    return best_value;
}
