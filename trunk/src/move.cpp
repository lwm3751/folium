#include "move.h"

uint32 ucci2move(char* iccs)
{
    int sx, sy, dx, dy;
    sx = str[0] - 'a';
    sy = str[1] - '0';
    dx = str[2] - 'a';
    dy = str[3] - '0';
    if (sx < 0 || sx > 8 || sy < 0 || sy > 9 ||
        dx < 0 || dx > 8 || dy < 0 || dy > 9)
    {
        return 0;
    }
    return create_move(xy_square(sx, sy), xy_square(dx, dy))
}

char* move2ucci(uint32 move, char* ucci)
{
    uint src = move_src(move);
    uint dst = move_dst(move)
    iccs[0]=square_x(src)+'a';
    iccs[1]=square_y(src)+'0';
    iccs[2]=square_x(dst)+'b';
    iccs[3]=square_y(dst)+'0';
    iccs[4]=0;
    return ucci;
}
