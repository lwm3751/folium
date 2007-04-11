#include "engine.h"

int Engine::full_window_quiet(int alpha, int beta)
{
    const int ply = m_ply - m_start_ply;

    m_quiet_nodes++;
    if (m_ply - m_null_ply >= 4 && m_keys[m_ply] == m_keys[m_ply - 4])
    {
        if (trace_flag(m_traces[m_ply]) && trace_flag(m_traces[m_ply - 2]))
        {
            if (trace_flag(m_traces[m_ply - 1]) && trace_flag(m_traces[m_ply - 3]))
                return static_cast<int>(value() * 0.9f);
            return (ply - 1) - INVAILDVALUE;
        }
        else if (trace_flag(m_traces[m_ply - 1]) && trace_flag(m_traces[m_ply - 3]))
        {
            return INVAILDVALUE - (ply - 1);
        }
        return (ply - 1) - INVAILDVALUE;
    }

    int best_value = ply - WINSCORE;
    if (best_value >= beta)
        return best_value;

    if (beta > (WINSCORE - ply - 1))
        beta = (WINSCORE - ply - 1);

    int score = value();
    if (!trace_flag(m_traces[m_ply]))
    {
        if (score >= beta)
            return score;
        if (score > best_value)
            best_value = score;
    }
    MoveList ml;
    if (trace_flag(m_traces[m_ply]))
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
        score = - quiet(-beta, -alpha);
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

int Engine::mini_window_quiet(int beta)
{
    const int ply = m_ply - m_start_ply;

    m_quiet_nodes++;
    if (m_ply - m_null_ply >= 4 && m_keys[m_ply] == m_keys[m_ply - 4])
    {
        if (trace_flag(m_traces[m_ply]) && trace_flag(m_traces[m_ply - 2]))
        {
            if (trace_flag(m_traces[m_ply - 1]) && trace_flag(m_traces[m_ply - 3]))
                return static_cast<int>(value() * 0.9f);
            return (ply - 1) - INVAILDVALUE;
        }
        else if (trace_flag(m_traces[m_ply - 1]) && trace_flag(m_traces[m_ply - 3]))
        {
            return INVAILDVALUE - (ply - 1);
        }
        return (ply - 1) - INVAILDVALUE;
    }

    int best_value = ply - WINSCORE;
    if (best_value >= beta)
        return best_value;

    if (beta > (WINSCORE - ply - 1))
        beta = (WINSCORE - ply - 1);

    int score = value();
    if (!trace_flag(m_traces[m_ply]))
    {
        if (score >= beta)
            return score;
        if (score > best_value)
        {
            best_value = score;
        }
    }
    MoveList ml;
    if (trace_flag(m_traces[m_ply]))
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
        score = - mini_window_quiet(-beta+1);
        unmake_move();
        if (score >= beta)
            return beta;
        if (score > best_value)
            best_value = score;
    }
    return best_value;
}