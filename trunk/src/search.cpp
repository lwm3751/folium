#include "engine.h"
#include "killer.h"
#include <iostream>
using namespace std;

const int LIMIT_DEPTH = 64;
const int NULL_DEPTH = 2;

static Killer killers[LIMIT_DEPTH+1];
int Engine::full(int depth, int alpha, int beta)
{
    if (m_stop)
        return - WINSCORE;
    const int ply = m_ply - m_start_ply;
    //Ñ­»·Ì½²â
    {
        int score = loop_value(ply);
        if (score != INVAILDVALUE)
            return score;
    }
    int best_value = ply - WINSCORE;
    //É±Æå¼ô²Ã
    if (best_value >= beta)
        return best_value;
    if (ply == LIMIT_DEPTH)
        return value();
    int extended = 0;
    //½«¾üÉìÑÓ
    //if (trace_flag(m_traces[m_ply]) || trace_dst(m_traces[m_ply]) == trace_dst(m_traces[m_ply-1]))
    if (trace_flag(m_traces[m_ply]))
    {
        ++depth;
        extended = 1;
        if (depth < 1)
            depth = 1;
    }
    //Ò¶×Ó½Úµã
    if (depth <= 0)
    {
        int score = value();
        if (score >= beta)
            return score;
        ++m_leaf_nodes;
        return leaf(alpha, beta);
    }
    ++m_tree_nodes;
    //hashÌ½²â
    Record& record = m_hash.record(m_keys[m_ply], m_xq.player());
    uint32 hash_move;
    {
        int score = record.probe(m_xq, depth, ply, beta-1, beta, hash_move, m_locks[m_ply]);
        if (hash_move && score != INVAILDVALUE)
        {
            m_hash_hit_nodes++;
            return score;
        }
    }

    uint32 best_move=0;
    Killer& killer=killers[ply];
    killers[ply+1].clear();
    MoveList ml;

    //null¼ô²Ã
    if (depth >= 2 && !extended && (value() - beta > 4) && beta < MATEVALUE && beta > -MATEVALUE)
    {
        if (full(depth > NULL_DEPTH ? (depth - NULL_DEPTH) : 1, beta-1, beta) > beta)
        {
            ++m_null_nodes;
            do_null();
            int score = - full(depth - NULL_DEPTH, -beta, -beta+1);
            undo_null();
            if (score >= beta)
            {
                ++m_null_cuts;
                return score;
            }
        }
    }

    bool found = false;
    //hash move
    if (hash_move && make_move(hash_move))
    {
        int score = - full(depth - 1, -beta, -alpha);
        unmake_move();
        if (score > best_value)
        {
            best_value = score;
            best_move = hash_move & 0x3fff;
            if (score >= beta)
            {
                if (m_stop)
                    return - WINSCORE;
                m_hash_move_cuts++;
                m_history.update_history(best_move, depth);
                record.store_beta(depth, ply, score, best_move, m_locks[m_ply]);
                if (!m_xq.is_good_cap(best_move))
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
    //good capture
    m_xq.generate_moves(ml, m_history);
    const uint size = ml.size();
    for (uint i = 0; i < size; ++i)
    {
        uint32 move = ml[i];
        if (!m_xq.is_good_cap(move))
            continue;
        ml[i] = 0;
        if (!make_move(move))
            continue;
        int score;
        if (found)
        {
            score = - full(depth - 1, -1-alpha, -alpha);
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
                record.store_beta(depth, ply, score, best_move, m_locks[m_ply]);
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
    //killermove
    for (uint i=0; i<2; ++i)
    {
        uint32 move = killer.killer(i);
        if (!is_legal_move(move))
            continue;
        if (!make_move(move))
            continue;
        int score;
        if (found)
        {
            score = - full(depth - 1, -1-alpha, -alpha);
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
                if (i == 0)
                    m_kill_cuts_1++;
                else
                    m_kill_cuts_2++;
                m_history.update_history(best_move, depth);
                record.store_beta(depth, ply, score, best_move, m_locks[m_ply]);
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
    //other
    for (uint i = 0; i < size; ++i)
    {
        uint32 move = ml[i];
        if (!move)
            continue;
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
        int score;
        if (found)
        {
            score = - full(depth - 1, -1-alpha, -alpha);
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
                //m_hash.store_beta(depth, ply, score, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
                record.store_beta(depth, ply, score, best_move, m_locks[m_ply]);
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
            record.store_alpha(depth, ply, best_value, best_move, m_locks[m_ply]);
        else
            record.store_pv(depth, ply, best_value, best_move, m_locks[m_ply]);
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

    //Ñ­»·Ì½²â
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
