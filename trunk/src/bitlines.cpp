#include "bitlines.h"
BitLines::BitLines()
{
    reset();
}
void BitLines::reset()
{
    for (int y =0; y < 10; ++y)
        m_xline[y].reset(0);
    for (int x = 0; x < 9; ++x)
        m_yline[x].reset(0);
}

