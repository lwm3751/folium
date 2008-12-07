#include "engine.h"
#include "xq_position_data.h"

#include <boost/format.hpp>

#include <ctime>
#include <vector>
#include <string>
#include <algorithm>
namespace folium
{

    Engine::Engine():
        m_debug(false),
        m_stop(true),
        m_ponder(false),
        m_depth(8),
        m_starttime(0.0f),
        m_mintime(0.0f),
        m_maxtime(0.0f),
        m_io(stdio()),
        m_hash(22)
    {
        load("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r");
    }

    void Engine::setxq(const XQ& xq)
    {
        m_xq = xq;

        m_traces[0] = create_trace(helper::status(m_xq), EmptyIndex, 0);
        m_ply = 0;
        m_start_ply = -1;
        m_keys[0] = 0UL;
        m_locks[0] = 0ULL;
        m_values[0] = 0L;

        for (uint i = 0; i < 32; ++i)
        {
            uint32 square = m_xq.piece(i);
            if (square == InvaildSquare)
                continue;
            assert(m_xq.square(square) == i);
            m_keys[0] ^= piece_key(i, square);
            m_locks[0] ^= piece_lock(i, square);
            m_values[0] += piece_value(i, square);
        }
    }

    bool Engine::load(const string& fen)
    {
        XQ xq;
        if (xq.set_fen(fen))
        {
            setxq(xq);
            return true;
        }
        return false;
    }

    void Engine::interrupt()
    {
        if (!m_ponder && now_real() >= m_maxtime)
        {
            m_stop = true;
        }
        if (!m_stop && m_io->readable())
        {
            string line = m_io->readline();
            if (line == "isready")
                m_io->writeline("readyok");
            else if (line == "stop")
                m_stop = true;
            else if (line == "ponderhit")
                m_ponder = false;
        }
    }

    bool Engine::readable()
    {
        return m_io->readable();
    }
    string Engine::readline()
    {
        return m_io->readline();
    }
    void Engine::writeline(const string& str)
    {
        m_io->writeline(str);
    }

    using namespace std;
    bool Engine::make_move(uint32 move)
    {
        uint src, dst, src_piece, dst_piece;
        src = move_src(move);
        dst = move_dst(move);
        assert(m_xq.is_legal_move(src, dst));
        src_piece = m_xq.square(src);
        dst_piece = m_xq.square(dst);
        assert(dst_piece != RedKingIndex && dst_piece != BlackKingIndex);

        if (!m_xq.do_move(src, dst))
        {
            return false;
        }

        ++m_ply;
        m_traces[m_ply] = create_trace(helper::status(m_xq), dst_piece, move);
        if (dst_piece == EmptyIndex)
        {
            m_keys[m_ply] = m_keys[m_ply-1]\
                            ^ piece_key(src_piece, src)
                            ^ piece_key(src_piece, dst);
            m_locks[m_ply] = m_locks[m_ply-1]\
                             ^ piece_lock(src_piece, src)\
                             ^ piece_lock(src_piece, dst);;
            m_values[m_ply] = m_values[m_ply-1]\
                              + piece_value(src_piece, dst)\
                              - piece_value(src_piece, src);
        }
        else
        {
            m_keys[m_ply] = m_keys[m_ply-1]\
                            ^ piece_key(src_piece, src)\
                            ^ piece_key(src_piece, dst)\
                            ^ piece_key(dst_piece, dst);
            m_locks[m_ply] = m_locks[m_ply-1]\
                             ^ piece_lock(src_piece, src)\
                             ^ piece_lock(src_piece, dst)\
                             ^ piece_lock(dst_piece, dst);
            m_values[m_ply] = m_values[m_ply-1]\
                              + piece_value(src_piece, dst)\
                              - piece_value(src_piece, src)\
                              - piece_value(dst_piece, dst);
        }
        return true;
    }

    void Engine::unmake_move()
    {
        assert (m_ply > 0);
        uint32 trace = m_traces[m_ply--];
        m_xq.undo_move(trace_src(trace), trace_dst(trace), trace_dst_piece(trace));
    }



    static vector<uint> generate_moves(const XQ& xq)
    {
        vector<uint> ml;
        uint own = xq.player();
        uint opp = 1 - own;
        uint idx;
        uint src, dst;
        if (own == Red)
        {
            idx = RedKingIndex;
            //red king
            src = xq.piece(RedKingIndex);
            const uint8 *pm = g_red_king_pawn_moves[src];
            dst = *pm++;
            while (dst != InvaildSquare)
            {
                if (xq.square_color(dst) != own)
                    ml.push_back(create_move(src, dst));
                dst = *pm++;
            }
            //red pawn
            for (uint i = RedPawnIndex1; i <= RedPawnIndex5; ++i)
            {
                src = xq.piece(i);
                if (src == InvaildSquare)
                    continue;
                pm = g_red_king_pawn_moves[src];
                dst = *pm++;
                while (dst != InvaildSquare)
                {
                    if (xq.square_color(dst) != own)
                        ml.push_back(create_move(src, dst));
                    dst = *pm++;
                }
            }
        }
        else
        {
            idx = BlackKingIndex;
            //black king
            src = xq.piece(BlackKingIndex);
            const uint8 *pm = g_black_king_pawn_moves[src];
            dst = *pm++;
            while (dst != InvaildSquare)
            {
                if (xq.square_color(dst) != own)
                    ml.push_back(create_move(src, dst));
                dst = *pm++;
            }
            //black pawn
            for (uint i = BlackPawnIndex1; i <= BlackPawnIndex5; ++i)
            {
                src = xq.piece(i);
                if (src == InvaildSquare)
                    continue;
                pm = g_black_king_pawn_moves[src];
                dst = *pm++;
                while (dst != InvaildSquare)
                {
                    if (xq.square_color(dst) != own)
                        ml.push_back(create_move(src, dst));
                    dst = *pm++;
                }
            }
        }
        //advisor
        for (uint i = 0; i < 2; ++i)
        {
            ++idx;
            src = xq.piece(idx);
            if (src == InvaildSquare)
                continue;
            const uint8 *pm = g_advisor_bishop_moves[src];
            dst = *pm++;
            while (dst != InvaildSquare)
            {
                if (xq.square_color(dst) != own)
                    ml.push_back(create_move(src, dst));
                dst = *pm++;
            }
        }
        //bishop
        for (uint i = 0; i < 2; ++i)
        {
            ++idx;
            src = xq.piece(idx);
            if (src == InvaildSquare)
                continue;
            const uint8 *pm = g_advisor_bishop_moves[src];
            dst = *pm++;
            while (dst != InvaildSquare)
            {
                if (xq.square_color(dst) != own && xq.square_is_empty((dst + src) >> 1))
                    ml.push_back(create_move(src, dst));
                dst = *pm++;
            }
        }
        //rook
        for (uint i = 0; i < 2; ++i)
        {
            uint dst;
            src = xq.piece(++idx);
            if (src == InvaildSquare)
                continue;
            dst = xq.nonempty_left_1(src);
            if (xq.square_color(dst) == opp)
                ml.push_back(create_move(src, dst));
            dst = xq.nonempty_right_1(src);
            if (xq.square_color(dst) == opp)
                ml.push_back(create_move(src, dst));
            dst = xq.nonempty_down_1(src);
            if (xq.square_color(dst) == opp)
                ml.push_back(create_move(src, dst));
            dst = xq.nonempty_up_1(src);
            if (xq.square_color(dst) == opp)
                ml.push_back(create_move(src, dst));
            for (uint tmp = xq.nonempty_left_1(src), dst = square_left(src);
                    dst != tmp;
                    dst = square_left(dst))
                ml.push_back(create_move(src, dst));
            for (uint tmp = xq.nonempty_right_1(src), dst = square_right(src);
                    dst != tmp;
                    dst = square_right(dst))
                ml.push_back(create_move(src, dst));
            for (uint tmp = xq.nonempty_down_1(src), dst = square_down(src);
                    dst != tmp;
                    dst = square_down(dst))
                ml.push_back(create_move(src, dst));
            for (uint tmp = xq.nonempty_up_1(src), dst = square_up(src);
                    dst != tmp;
                    dst = square_up(dst))
                ml.push_back(create_move(src, dst));
        }
        //knight
        for (uint i = 0; i < 2; ++i)
        {
            ++idx;
            src = xq.piece(idx);
            if (src == InvaildSquare)
                continue;
            const uint16 *pm = g_kinght_moves[src];
            dst = *pm++;
            //23130 = ((InvaildSquare << 8) | InvaildSquare)
            while (dst != 23130UL)
            {
                uint leg = (dst & 0xff00) >> 8;
                dst &= 0xff;
                if (xq.square_is_empty(leg) && xq.square_color(dst) != own)
                    ml.push_back(create_move(src, dst));
                dst = *pm++;
            }
        }
        //cannon
        for (uint i = 0; i < 2; ++i)
        {
            uint dst;
            src = xq.piece(++idx);
            if (src == InvaildSquare)
                continue;
            dst = xq.nonempty_left_2(src);
            if (xq.square_color(dst) == opp)
                ml.push_back(create_move(src, dst));
            dst = xq.nonempty_right_2(src);
            if (xq.square_color(dst) == opp)
                ml.push_back(create_move(src, dst));
            dst = xq.nonempty_down_2(src);
            if (xq.square_color(dst) == opp)
                ml.push_back(create_move(src, dst));
            dst = xq.nonempty_up_2(src);
            if (xq.square_color(dst) == opp)
                ml.push_back(create_move(src, dst));
            for (uint tmp = xq.nonempty_left_1(src), dst = square_left(src);
                    dst != tmp;
                    dst = square_left(dst))
                ml.push_back(create_move(src, dst));
            for (uint tmp = xq.nonempty_right_1(src), dst = square_right(src);
                    dst != tmp;
                    dst = square_right(dst))
                ml.push_back(create_move(src, dst));
            for (uint tmp = xq.nonempty_down_1(src), dst = square_down(src);
                    dst != tmp;
                    dst = square_down(dst))
                ml.push_back(create_move(src, dst));
            for (uint tmp = xq.nonempty_up_1(src), dst = square_up(src);
                    dst != tmp;
                    dst = square_up(dst))
                ml.push_back(create_move(src, dst));
        }
        return ml;
    }

    static vector<uint> generate_root_move(XQ& xq, const set<uint>& ban)
    {
        vector<uint> r;
        vector<uint> ml = generate_moves(xq);
        for (uint i = 0; i < ml.size(); ++i)
        {
            uint move = ml[i];
            uint dst_piece = xq.square(move_dst(move));
            if (dst_piece == RedKingIndex || dst_piece == BlackKingIndex)
            {
                r.clear();
                r.push_back(move);
                return r;
            }
            if (ban.find(move) != ban.end())
                continue;
            if (xq.do_move(move_src(move), move_dst(move)))
            {
                r.push_back(move);
                xq.undo_move(move_src(move), move_dst(move), dst_piece);
            }
        }
        return r;
    }
    uint32 Engine::search(set<uint> ban)
    {
        //writeline(str( boost::format("info depth %d mintime %f maxtime %f") % m_depth % m_mintime % m_maxtime ));

        m_interrupt = 0;

        m_tree_nodes = 0;
        m_leaf_nodes = 0;
        m_quiet_nodes = 0;
        m_hash_hit_nodes = 0;
        m_hash_move_cuts = 0;
        m_kill_cuts_1 = 0;
        m_kill_cuts_2 = 0;
        m_null_nodes = 0;
        m_null_cuts = 0;

        m_history.clear();
        m_hash.clear();

        m_null_ply = 0;
        m_start_ply = m_ply;

        int best_value;
        vector<uint> ml = generate_root_move(m_xq, ban);
        vector<uint> bests;
        for (sint i = 1;
            !m_stop && i < m_depth  && now_real() < m_mintime;
            ++i)
        {
            if (ml.size() < 2)
            {
                bests = ml;
                break;
            }
            best_value = -INVAILDVALUE;
            vector<uint> olds;
            bests.swap(olds);
            for (uint j = 0; j < olds.size(); ++j)
            {
                int score;
                uint32 move = olds[j];
                make_move(move);
                if (best_value != -INVAILDVALUE)
                {
                    score = - full(i, -1-best_value, -best_value);
                    if (score > best_value)
                        score = - full(i, -WINSCORE, -1-best_value);
                }
                else
                {
                    score = - full(i, -WINSCORE, WINSCORE);
                }
                unmake_move();
                writeline(str( boost::format("info move %s depth %d score %d") % move2ucci(move) %i % score));
                if (score >= best_value)
                {
                    if (score > best_value)
                        bests.clear();
                    if (find(bests.begin(), bests.end(), move) == bests.end())
                    {
                        bests.push_back(move);
                    }
                    best_value = score;
                }
            }
            for (uint j = 0; j < ml.size(); ++j)
            {
                uint32 move = ml[j];
                if (!make_move(move))
                {
                    ml[j] = 0;
                    continue;
                }
                int score;
                if (best_value != -INVAILDVALUE)
                {
                    score = - full(i, -1-best_value, -best_value);
                    if (score > best_value)
                        score = - full(i, -WINSCORE, -best_value);
                }
                else
                {
                    score = - full(i, -WINSCORE, WINSCORE);
                }
                unmake_move();
                if (m_stop)
                    break;
                if (score > best_value)
                {
                    writeline(str( boost::format("info move %s depth %d score %d") % move2ucci(move) %i % score));
                    if (score > best_value)
                        bests.clear();
                    if (find(bests.begin(), bests.end(), move) == bests.end())
                    {
                        bests.push_back(move);
                    }
                    best_value = score;
                }
                else if (score < -MATEVALUE)
                {
                    ml[j] = 0;
                }
            }

            if (best_value > MATEVALUE || best_value < -MATEVALUE)
                break;
            ml.erase(remove(ml.begin(), ml.end(), 0), ml.end());
        }
        if (bests.empty())
            return 0;
        return bests[clock()%bests.size()];
    }
}
