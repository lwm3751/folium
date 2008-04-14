#include "generator.h"

uint32 Generator::next()
{
    switch(m_stage)
    {
    case 0:
        m_stage = 1;
        if (m_hash_move)
            return m_hash_move;
    case 1:
        m_stage = 2;
        m_index = 0;
        m_xq.generate_moves(m_ml, m_history);
    case 2:
        while(m_index < m_ml.size())
        {
            uint32 move = m_ml[m_index];
            if (m_xq.is_good_cap(move))
            {
                m_ml[m_index] = 0;
                m_index++;
                return move;
            }
            m_index++;
        }
        m_stage = 3;
        m_index = 0;
    case 3:
        while(m_index < 2)
        {
            uint move = m_killer.killer(m_index);
            m_index++;
            if (move && m_xq.is_legal_move(move))
                return move;
        }
        m_stage = 4;
        m_index = 0;
    case 4:
        while(m_index < m_ml.size())
        {
            uint32 move = m_ml[m_index];
            if (!move)
            {
                m_index++;
                continue;
            }
            for (uint j = m_index + 1; j < m_ml.size(); ++j)
            {
                if (m_ml[j] > move)
                {
                    m_ml[m_index] = m_ml[j];
                    m_ml[j] = move;
                    move = m_ml[m_index];
                }
            }
            m_index++;
            return move;
        }
    }
    return 0;
}
