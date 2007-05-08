#ifndef _HASH_H_
#define _HASH_H_

#include "defines.h"

const int HALPHA = 1;

const int WINSCORE = 10000;
const int MATEVALUE = 9000;
const int INVAILDVALUE = 20000;

struct Record
{
    uint64 lock;
    sint8 depth;
    sint8 flag;
    uint16 move;
    sint32 score;
};
class HashTable
{
public:
    HashTable(uint32 power=22);
    ~HashTable();
    void clear();
    int probe(int depth, int ply, int alpha, int beta,\
            uint32& move, uint player, const uint32 &key, const uint64 &lock);
    void store_beta(int depth, int ply, int score, uint32 move, uint player, const uint32 &key, const uint64 &lock);
    void store_alpha(int depth, int ply, int score, uint32 move, uint player, const uint32 &key, const uint64 &lock);
    void store_pv(int depth, int ply, int score, uint32 move, uint player, const uint32 &key, const uint64 &lock);
private:
    uint32 m_size;
    uint32 m_mask;
    Record* m_records[2];
};

#endif //_HASH_H_
