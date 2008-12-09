#include "move.h"
#include "xq_data.h"
namespace folium
{
    uint32 ucci2move(const string& ucci)
    {
        int sx, sy, dx, dy;
        sx = ucci[0] - 'a';
        sy = ucci[1] - '0';
        dx = ucci[2] - 'a';
        dy = ucci[3] - '0';
        if (sx < 0 || sx > 8 || sy < 0 || sy > 9 ||
                dx < 0 || dx > 8 || dy < 0 || dy > 9)
        {
            return 0;
        }
        return create_move(xy_square(sx, sy), xy_square(dx, dy));
    }

    string move2ucci(uint32 move)
    {
        string ucci;
        uint src = move_src(move);
        uint dst = move_dst(move);
        ucci.push_back(static_cast<char>(square_x(src)+'a'));
        ucci.push_back(static_cast<char>(square_y(src)+'0'));
        ucci.push_back(static_cast<char>(square_x(dst)+'a'));
        ucci.push_back(static_cast<char>(square_y(dst)+'0'));
        return ucci;
    }

}//namespace folium
