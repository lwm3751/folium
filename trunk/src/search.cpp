#include "engine.h"

int Engine::full(int depth, int alpha, int beta)
{
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

    if (beta > (WINSCORE - ply - 1))
        beta = (WINSCORE - ply - 1);

    if (!flag)
        depth--;
    int score;
    score = m_hash.probe(depth, ply, beta, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
    if (best_move && make_move(best_move))
    {
        score = - full(depth, -beta, -alpha);
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
                m_hash.store(depth, ply, score, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
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
        ml[i] = m_history.update_move(ml[i]);
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
            score = - mini(depth, -alpha+1);
            if ((score > alpha) && (score < beta))
                score = - full(depth, -beta, -alpha);
        }
        else
        {
            score = - full(depth, -beta, -alpha);
        }
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
                m_hash.store(depth, ply, score, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
                return beta;
            }
            if (score > alpha)
            {
                found = true;
                alpha = score;
            }
        }
    }
    m_history.update_history(best_move, depth);
    return best_value;
}
int Engine::mini(int depth, int beta)
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

    if (beta > (WINSCORE - ply - 1))
        beta = (WINSCORE - ply - 1);

    if (!flag)
        depth--;
    int score;
    score = m_hash.probe(depth, ply, beta, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
    if (score != INVAILDVALUE)
    {
        m_hash_hit_nodes++;
        return score;
    }
    if (best_move && make_move(best_move))
    {
        score = - mini(depth, -beta+1);
        unmake_move();
        if (m_stop)
            return - WINSCORE;
        if (score > best_value)
        {
            best_value = score;
            if (score >= beta)
            {
                m_hash_move_cuts++;
                m_history.update_history(best_move, depth);
                m_hash.store(depth, ply, score, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
                return score;
            }
        }
    }

    MoveList ml;
    m_xq.generate_moves(ml);
    uint size = ml.size();
    for (uint i = 0; i < size; ++i)
    {
        ml[i] = m_history.update_move(ml[i]);
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
            if (score >= beta)
            {
                m_history.update_history(best_move, depth);
                m_hash.store(depth, ply, score, best_move, m_xq.player(), m_keys[m_ply], m_locks[m_ply]);
                return beta;
            }
        }
    }
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

    if (beta > (WINSCORE - ply - 1))
        beta = (WINSCORE - ply - 1);

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
    {
        m_xq.generate_moves(ml);
    }
    else
    {
        m_xq.generate_capture_moves(ml);
        uint size = ml.size();
        for (uint i = 0; i < size; ++i)
        {
            uint32 move = ml[i];
            ml[i] = m_history.update_capture_move(move, m_xq.square(move_src(move)), m_xq.square(move_dst(move)));
        }
    }

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
