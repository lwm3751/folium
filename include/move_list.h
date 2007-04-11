#ifndef _MOVE_LIST_H_
#define _MOVE_LIST_H_

#include <algorithm>
#include <functional>

#include "defines.h"

const uint32 MaxMoveNumber = 128;
class MoveList
{
private:
    uint32 ml[MaxMoveNumber];
    uint32 length;
public:
    MoveList();
    uint32 size() const;
    void clear();
    uint32& operator[](uint32 index);
    void push(uint32 src, uint32 dst);
    void sort();
};
inline MoveList::MoveList():length(0)
{
}
inline uint32 MoveList::size() const
{
    return length;
}
inline void MoveList::clear()
{
    length = 0;
}
inline uint32& MoveList::operator[](uint32 index)
{
    assert(index < length);
    return ml[index];
}
inline void MoveList::push(uint32 src, uint32 dst)
{
    ml[length++] = src | (dst << 7);
}
inline void MoveList::sort()
{
    ::std::sort(ml, ml + length, ::std::greater<uint32>());
}
#endif //_MOVE_LIST_H_
