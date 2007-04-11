#include "hash.h"
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
        m_records[0][i].depth = -1;
    for (uint32 i = 0; i < m_size; ++i)
        m_records[1][i].depth = -1;
}

int HashTable::probe(int depth, int ply, int beta,
              uint32& move, uint player, const uint32 &key, const uint64 &lock)
{
    Record& record = m_records[player][key & m_mask];
    if (record.lock == lock && record.flag)
    {
        int score = record.score;
        if (record.score > MATEVALUE)
            return record.score - ply;
        else if (record.score < -MATEVALUE)
            score = record.score + ply;
        if (record.depth >= depth)
        {
            if (score >= beta)
                return score;
        }
        move = record.move;
    }
    else
    {
        move = 0;
    }
    return INVAILDVALUE;
}

void HashTable::store(int depth, int ply, int score, uint32 move,
                uint player, const uint32 &key, const uint64 &lock)
{
    Record& record = m_records[player][key & m_mask];
    if (score > WINSCORE || score < -WINSCORE)
        return;
    if ((record.flag) && record.depth > depth)
        return;
    if (score > MATEVALUE)
        score += ply;
    else if (score < -MATEVALUE)
        score -= ply;
    record.lock = lock;
    record.depth = depth;
    record.flag = 1;
    record.move = (uint16) move;
    record.score = score;
}
