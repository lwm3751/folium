#include "bitlines.h"
BitLines::BitLines()
{
    reset();
}
BitLines::BitLines(const BitLines& bitlines)
{
    for (int y =0; y < 10; ++y)
        m_xline[y].reset(bitlines.m_xline[y]);
    for (int x = 0; x < 9; ++x)
        m_yline[x].reset(bitlines.m_yline[x]);
}
void BitLines::reset()
{
    for (int y =0; y < 10; ++y)
        m_xline[y].reset(512);
    for (int x = 0; x < 9; ++x)
        m_yline[x].reset(0);
}

