#include "hash.h"
const int ALPHA = 1;
const int BETA = 2;
const int PV = 3;
HashTable::HashTable(uint32 power)
{
    m_size = 1 << (power -  1);
    m_mask = m_size - 1;
    m_records[0] = new Record[m_size];
    m_records[1] = new Record[m_size];
    for (uint32 i = 0; i < m_size; ++i)
    {
        m_records[0][i].flag = 0;
    }
    for (uint32 i = 0; i < m_size; ++i)
    {
        m_records[1][i].flag = 0;
    }
}

HashTable::~HashTable()
{
    delete[] m_records[0];
    delete[] m_records[1];
}

void HashTable::clear()
{
    for (uint32 i = 0; i < m_size; ++i)
    {
        uint flag = m_records[0][i].flag;
        if (flag > PV)
        {
            m_records[0][i].flag = flag & PV;
        }
        else
        {
            m_records[0][i].flag = 0;
        }
    }
    for (uint32 i = 0; i < m_size; ++i)
    {
        uint flag = m_records[1][i].flag;
        if (flag > PV)
        {
            m_records[1][i].flag = flag & PV;
        }
        else
        {
            m_records[1][i].flag = 0;
        }
    }
}

int HashTable::probe(int depth, int ply, int alpha, int beta,\
              uint32& move, uint player, const uint32 &key, const uint64 &lock)
{
    Record& record = m_records[player][key & m_mask];
    uint flag = record.flag & PV;
    if (record.lock == lock && flag)
    {
        record.flag = flag | 4;
        move = record.move;
        int score = record.score;
        if (score == INVAILDVALUE)
            return INVAILDVALUE;
        else if (record.score > MATEVALUE)
            return record.score - ply;
        else if (record.score < -MATEVALUE)
            return record.score + ply;
        if (record.depth >= depth)
        {
            switch(flag & PV)
            {
            case ALPHA:
                if (score <= alpha)
                    return score;
                break;
            case BETA:
                if (score >= beta)
                    return score;
                break;
            case PV:
                return score;
            }
        }
    }
    else
    {
        move = 0;
    }
    return INVAILDVALUE;
}
void HashTable::store_alpha(int depth, int ply, int score, uint32 move,
                uint player, const uint32 &key, const uint64 &lock)
{
    Record& record = m_records[player][key & m_mask];
    if ((record.flag > PV) && record.depth > depth)
        return;
    if (score > MATEVALUE || score < -WINSCORE)
        score = INVAILDVALUE;
    else if (score < -MATEVALUE)
        score -= ply;
    record.lock = lock;
    record.depth = depth;
    record.flag = ALPHA | 4;
    record.move = (uint16) move;
    record.score = score;
}

void HashTable::store_beta(int depth, int ply, int score, uint32 move,
                uint player, const uint32 &key, const uint64 &lock)
{
    Record& record = m_records[player][key & m_mask];
    if ((record.flag > PV) && record.depth > depth)
        return;
    if (score > WINSCORE  || score < -MATEVALUE)
        score = INVAILDVALUE;
    else if (score > MATEVALUE)
        score += ply;
    record.lock = lock;
    record.depth = depth;
    record.flag = BETA | 4;
    record.move = (uint16) move;
    record.score = score;
}

void HashTable::store_pv(int depth, int ply, int score, uint32 move,
                uint player, const uint32 &key, const uint64 &lock)
{
    Record& record = m_records[player][key & m_mask];
    if ((record.flag > PV) && record.depth > depth)
        return;
    if (score > WINSCORE  || score < -WINSCORE)
        score = INVAILDVALUE;
    else if (score > MATEVALUE)
        score += ply;
    else if (score < -MATEVALUE)
        score -= ply;
    record.lock = lock;
    record.depth = depth;
    record.flag = PV | 4;
    record.move = (uint16) move;
    record.score = score;
}
