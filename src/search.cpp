#include "engine.h"
#include <iostream>
using namespace std;
const int NULL_DEPTH = 2;

int Engine::full(uint32& killer, int depth, int alpha, int beta)
{
    if (m_stop)
        return - WINSCORE;
    if (depth <= 0)
        return quies(alpha, beta);
    bool found = false;
    const int ply = m_ply - m_start_ply;
    const int flag = trace_flag(m_traces[m_ply]);

    ++m_tree_nodes;
    //Ñ­»·Ì½²â
    if ((m_ply - m_null_ply) >= 4 && m_keys[m_ply] == m_keys[m_ply - 4])
    {
        if (flag && trace_flag(m_traces[m_ply - 2]))
        {
            if (trace_flag(m_traces[m_ply - 1]) && trace_flag(m_traces[m_ply - 3]))
                return static_cast<int>(value() * 0.9f);
            return INVAILDVALUE - (ply - 1);
        }
        else if (trace_flag(m_traces[m_ply - 1]) && trace_flag(m_traces[m_ply - 3]))
        {
            return (ply - 1) - INVAILDVALUE;
        }
        return static_cast<int>(value() * 0.9f);
    }

    int best_value = ply - WINSCORE;
    //É±Æå¼ô²Ã
    if (best_value >= beta)
        return best_value;
    uint32 best_move=0;
    uint32 sub_killer=0;
    int score;
    MoveList ml;

    //½«¾üÉìÑÓ
    if (!flag)
        depth--;

    //Ì½²âhashmove ÒÔ¼°killermove
    //*
    uint32 hash_move;
    m_hash.probe(depth, ply, alpha, beta, hash_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
    if (m_xq.is_legal_move(hash_move))
        ml.push(hash_move);
    if (m_xq.is_legal_move(killer))
        ml.push(killer);
    if (m_xq.is_legal_move(killer>>16))
        ml.push(killer>>16);
    for (uint i=0; i<ml.size(); ++i)
    {
        uint32 move = ml[i];
        if (m_xq.player() != piece_color(m_xq.square(move_src(move))))
            continue;
        if (!make_move(move))
            continue;
        if (found)
        {
            score = - mini(sub_killer, depth, -alpha);
            if ((score > alpha) && (score < beta))
                score = - full(sub_killer, depth, -beta, -alpha);
        }
        else
            score = - full(sub_killer, depth, -beta, -alpha);
        unmake_move();
        if (score > best_value)
        {
            best_value = score;
            best_move = move & 0x3fff;
            if (score >= beta)
            {
                if (m_stop)
                    return - WINSCORE;
                if (move == hash_move)
                    m_hash_move_cuts++;
                else if (move = killer)
                    m_kill_cuts_1++;
                else
                    m_kill_cuts_2++;
                m_history.update_history(best_move, depth);
                m_hash.store_beta(depth, ply, score, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
                if ((killer&0xffff) != best_move)
                    killer = best_move | (killer << 16);
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
            score = - mini(sub_killer, depth, -alpha);
            if ((score > alpha) && (score < beta))
                score = - full(sub_killer, depth, -beta, -alpha);
        }
        else
            score = - full(sub_killer, depth, -beta, -alpha);
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
                if ((killer&0xffff) != best_move)
                    killer = best_move | (killer << 16);
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
    m_history.update_history(best_move, depth);
    if (best_move && (killer&0xffff) != best_move)
        killer = best_move | (killer << 16);
    if (!found)
        m_hash.store_alpha(depth, ply, best_value, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
    else
        m_hash.store_pv(depth, ply, best_value, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
    return best_value;
}
int Engine::mini(uint32& killer, int depth, int beta, bool do_null)
{
    if (depth <= 0)
        return quies(beta-1, beta);
    const int ply = m_ply - m_start_ply;
    const int flag = trace_flag(m_traces[m_ply]);

    //Ñ­»·Ì½²â
    ++m_tree_nodes;
    if ((m_ply - m_null_ply) >= 4 && m_keys[m_ply] == m_keys[m_ply - 4])
    {
        if (flag && trace_flag(m_traces[m_ply - 2]))
        {
            if (trace_flag(m_traces[m_ply - 1]) && trace_flag(m_traces[m_ply - 3]))
                return static_cast<int>(value() * 0.9f);
            return INVAILDVALUE - (ply - 1);
        }
        else if (trace_flag(m_traces[m_ply - 1]) && trace_flag(m_traces[m_ply - 3]))
        {
            return (ply - 1) - INVAILDVALUE;
        }
        return static_cast<int>(value() * 0.9f);
    }

    int best_value = ply - WINSCORE;
    //É±Æå¼ô²Ã
    if (best_value >= beta)
        return best_value;
    uint32 best_move=0;
    uint32 sub_killer=0;
    int score;
    MoveList ml;

    //½«¾üÉìÑÓ
    if (!flag)
        depth--;

    //hashÌ½²â
    uint32 hash_move;
    score = m_hash.probe(depth, ply, beta-1, beta, hash_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
    if (m_xq.is_legal_move(hash_move) && m_xq.player() == piece_color(m_xq.square(move_src(hash_move))))
    {
        if (score != INVAILDVALUE)
        {
            m_hash_hit_nodes++;
            return score;
        }
        ml.push(hash_move);
    }

    //null¼ô²Ã
    if (depth >= 2 && flag == 0 && do_null && (value() - beta > 4))
    {
        ++m_null_nodes;
        uint32 tmp_killer;
        make_null();
        score = - mini(tmp_killer, depth - NULL_DEPTH, -beta+1, false);
        unmake_null();
        if (score >= beta)
        {
            if (mini(killer, depth > NULL_DEPTH ? (depth - NULL_DEPTH) : 1, -beta+1, false) >= beta)
            {
                ++m_null_cuts;
                return score;
            }
        }
    }

    //Ì½²âhashmove ÒÔ¼°killermove
    //*
    if (m_xq.is_legal_move(killer))
        ml.push(killer);
    if (m_xq.is_legal_move(killer>>16))
        ml.push(killer>>16);//*/
    for (uint i=0; i<ml.size(); ++i)
    {
        uint32 move = ml[i];
        assert(m_xq.is_legal_move(move));
        assert(piece_color(m_xq.square(move_src(move)))==m_xq.m_player);
        if (m_xq.player() != piece_color(m_xq.square(move_src(move))))
            continue;
        if (!make_move(move))
            continue;
        score = - mini(sub_killer, depth, -beta+1);
        unmake_move();
        if (score > best_value)
        {
            best_value = score;
            best_move = move & 0x3fff;
            if (score >= beta)
            {
                if (m_stop)
                    return - WINSCORE;
                if (move == hash_move)
                    m_hash_move_cuts++;
                else if (move = killer)
                    m_kill_cuts_1++;
                else
                    m_kill_cuts_2++;
                m_history.update_history(best_move, depth);
                m_hash.store_beta(depth, ply, score, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
            if ((killer&0xffff) != best_move)
                killer = best_move | (killer << 16);
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
        score = - mini(sub_killer, depth, -beta+1);
        unmake_move();

        if (m_stop)
            return - WINSCORE;

        if (score > best_value)
        {
            best_value = score;
            best_move = move;
            if (best_value >= beta)
            {
                if (m_stop)
                    return -WINSCORE;
                m_history.update_history(best_move, depth);
                m_hash.store_beta(depth, ply, best_value, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
                if ((killer&0xffff) != best_move)
                    killer = best_move | (killer << 16);
                return beta;
            }
        }
    }
    if (m_stop)
        return - WINSCORE;
    m_history.update_history(best_move, depth);
    m_hash.store_alpha(depth, ply, best_value, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
    return best_value;
}

int Engine::quies(int alpha, int beta)
{
    const int ply = m_ply - m_start_ply;
    const int flag = trace_flag(m_traces[m_ply]);

    m_quiet_nodes++;
    if ((m_ply - m_null_ply) >= 4 && m_keys[m_ply] == m_keys[m_ply - 4])
    {
        if (flag && trace_flag(m_traces[m_ply - 2]))
        {
            if (trace_flag(m_traces[m_ply - 1]) && trace_flag(m_traces[m_ply - 3]))
                return static_cast<int>(value() * 0.9f);
            return INVAILDVALUE - (ply - 1);
        }
        else if (trace_flag(m_traces[m_ply - 1]) && trace_flag(m_traces[m_ply - 3]))
        {
            return (ply - 1) - INVAILDVALUE;
        }
        return static_cast<int>(value() * 0.9f);
    }

    int score, best_value;
    best_value = ply - WINSCORE;
    if (best_value >= beta)
        return best_value;

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
