#include "engine.h"

const int NULL_DEPTH = 2;

int Engine::full(int depth, int alpha, int beta)
{
    if (m_stop)
        return - WINSCORE;
    if (depth <= 0)
        return quies(alpha, beta);
    bool found = false;
    const int ply = m_ply - m_start_ply;
    const int flag = trace_flag(m_traces[m_ply]);

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
    uint32 best_move;
    if (best_value >= beta)
    {
        return best_value;
    }

    if (!flag)
        depth--;
    const int old_alpha = alpha;
    int score = m_hash.probe(depth, ply, alpha, beta, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
    if (m_xq.is_legal_move(best_move) && make_move(best_move))
    {
        score = - full(depth, -beta, -alpha);
        unmake_move();
        if (score > best_value)
        {
            best_value = score;
            if (score >= beta)
            {
                if (m_stop)
                    return -WINSCORE;
                m_hash_move_cuts++;
                m_history.update_history(best_move, depth);
                m_hash.store_beta(depth, ply, score, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
                return score;
            }
            if (score > alpha)
            {
                found = true;
                alpha = score;
            }
        }
    }

    MoveList ml;
    m_xq.generate_moves(ml);
    uint size = ml.size();
    for (uint i = 0; i < size; ++i)
    {
        uint32 move = ml[i];
        ml[i] = m_history.update_move(move, m_xq.square(move_src(move)), m_xq.square(move_dst(move)));
    }
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
            score = - mini(depth, -alpha);
            if ((score > alpha) && (score < beta))
                score = - full(depth, -beta, -alpha);
        }
        else
            score = - full(depth, -beta, -alpha);
        unmake_move();

        if (score > best_value)
        {
            best_value = score;
            best_move = move;
            if (score >= beta)
            {
                if (m_stop)
                    return - WINSCORE;
                m_history.update_history(best_move, depth);
                m_hash.store_beta(depth, ply, score, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
                return beta;
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
    if (best_value <= old_alpha)
        m_hash.store_alpha(depth, ply, best_value, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
    else
        m_hash.store_pv(depth, ply, best_value, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
    return best_value;
}
int Engine::mini(int depth, int beta, bool do_null)
{
    if (depth <= 0)
        return quies(beta-1, beta);
    const int ply = m_ply - m_start_ply;
    const int flag = trace_flag(m_traces[m_ply]);

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
    uint32 best_move;
    if (best_value >= beta)
        return best_value;

    if (!flag)
        depth--;
    int score;
    score = m_hash.probe(depth, ply, beta-1, beta, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
    if (score != INVAILDVALUE && m_xq.is_legal_move(best_move))
    {
        m_hash_hit_nodes++;
        return score;
    }
    //*
    if (depth >= 2 && flag == 0 && do_null && (value() - beta > 4))
    {
        ++m_null_nodes;
        make_null();
        score = - mini(depth - NULL_DEPTH, -beta+1, false);
        unmake_null();
        if (score >= beta)
        {
            if (mini(depth > NULL_DEPTH ? (depth - NULL_DEPTH) : 1, -beta+1, false) >= beta)
            {
                ++m_null_cuts;
                return score;
            }
        }
    }//*/
    
    if (m_xq.is_legal_move(best_move) && make_move(best_move))
    {
        score = - mini(depth, -beta+1);
        unmake_move();
        if (score > best_value)
        {
            best_value = score;
            if (score >= beta)
            {
                if (m_stop)
                    return - WINSCORE;
                m_hash_move_cuts++;
                m_history.update_history(best_move, depth);
                m_hash.store_beta(depth, ply, score, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
                return score;
            }
        }
    }

    MoveList ml;
    m_xq.generate_moves(ml);
    uint size = ml.size();
    for (uint i = 0; i < size; ++i)
    {
        uint32 move = ml[i];
        ml[i] = m_history.update_move(move, m_xq.square(move_src(move)), m_xq.square(move_dst(move)));
    }
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
        score = - mini(depth, -beta+1);
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
        m_xq.generate_moves(ml);
    else
        m_xq.generate_capture_moves(ml);
    uint size = ml.size();
    for (uint i = 0; i < size; ++i)
    {
        uint32 move = ml[i];
        ml[i] = m_history.update_move(move, m_xq.square(move_src(move)), m_xq.square(move_dst(move)));
    }
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
