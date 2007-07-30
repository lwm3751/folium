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
    const int ply = m_ply - m_start_ply;

    //ѭ��̽��
    {
        int score = loop_value(ply);
        if (score != INVAILDVALUE)
            return score;
    }

    int best_value = ply - WINSCORE;

    //ɱ�����
    if (best_value >= beta)
        return best_value;

    if (ply == LIMIT_DEPTH)
        return value();

    uint32 best_move=0;
    Killer& killer=killers[ply];
    killers[ply+1].clear();
    int score;
    MoveList ml;

    int extended = 0;
    //��������
    if (trace_flag(m_traces[m_ply]))
    {
        ++depth;
        extended = 1;
        if (depth < 1)
            depth = 1;
    }

    if (depth <= 0)
    {
        int score = value();
        if (score >= beta)
            return score;
        ++m_leaf_nodes;
        return leaf(alpha, beta);
    }
    ++m_tree_nodes;

    bool found = false;
    //̽��hashmove �Լ�killermove
    //*
    uint32 hash_move;
    m_hash.probe(depth, ply, alpha, beta, hash_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
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
            score = - mini(depth - 1, -alpha);
            if ((score > alpha) && (score < beta))
                score = - full(depth - 1, -beta, -alpha);
        }
        else
            score = - full(depth - 1, -beta, -alpha);
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
                m_history.update_history(best_move, depth);
                m_hash.store_beta(depth, ply, score, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
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
            score = - mini(depth - 1, -alpha);
            if ((score > alpha) && (score < beta))
                score = - full(depth - 1, -beta, -alpha);
        }
        else
            score = - full(depth - 1, -beta, -alpha);
        unmake_move();

        if (score > best_value)
        {
            best_value = score;
            best_move = move & 0x3fff;
            if (score >= beta)
            {
                if (m_stop)
                    return - WINSCORE;
                m_history.update_history(best_move, depth);
                m_hash.store_beta(depth, ply, score, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
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
        m_history.update_history(best_move, depth);
        if (!found)
            m_hash.store_alpha(depth, ply, best_value, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
        else
            m_hash.store_pv(depth, ply, best_value, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
    }
    return best_value;
}
int Engine::mini(int depth, int beta, bool do_null)
{
    if (m_stop)
        return - WINSCORE;
    const int ply = m_ply - m_start_ply;
    const int flag = trace_flag(m_traces[m_ply]);

    //ѭ��̽��
    {
        int score = loop_value(ply);
        if (score != INVAILDVALUE)
            return score;
    }

    int best_value = ply - WINSCORE;
    //ɱ�����
    if (best_value >= beta)
        return best_value;

    if (ply == LIMIT_DEPTH)
        return value();

    Killer& killer=killers[ply];
    killers[ply+1].clear();

    uint32 best_move=0;
    int score;
    MoveList ml;
    
    int extended = 0;
    //��������
    if (trace_flag(m_traces[m_ply]))
    {
        ++depth;
        extended = 1;
        if (depth < 1)
            depth = 1;
    }

    if (depth <= 0)
    {
        int score = value();
        if (score >= beta)
            return score;
        ++m_leaf_nodes;
        return leaf(beta-1, beta);
    }
    ++m_tree_nodes;

    //hash̽��
    uint32 hash_move;
    score = m_hash.probe(depth, ply, beta-1, beta, hash_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
    if (m_xq.is_legal_move(hash_move) && m_xq.player() == piece_color(m_xq.square(move_src(hash_move))))
    {
        if (score != INVAILDVALUE)
        {
            m_hash_hit_nodes++;
            return score;
        }
    }

    //̽��hashmove �Լ�killermove
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
        score = - mini(depth - 1, -beta + 1);
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
                m_history.update_history(best_move, depth);
                m_hash.store_beta(depth, ply, score, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
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
        score = - mini(depth - 1, -beta+1);
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
                m_history.update_history(best_move, depth);
                m_hash.store_beta(depth, ply, best_value, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
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
        m_history.update_history(best_move, depth);
        m_hash.store_alpha(depth, ply, best_value, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
    }
    return best_value;
}

int Engine::leaf(int alpha, int beta)
{
    const int ply = m_ply - m_start_ply;
    m_leaf_nodes++;
    int best_value = ply - WINSCORE;
    {
        int score = value();
        if (score > best_value)
            best_value = score;
    }
    MoveList ml;
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
        int score = - quies(-beta, -alpha);
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
int Engine::quies(int alpha, int beta)
{
    const int ply = m_ply - m_start_ply;
    const int flag = trace_flag(m_traces[m_ply]);

    //ѭ��̽��
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
    m_quiet_nodes++;
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