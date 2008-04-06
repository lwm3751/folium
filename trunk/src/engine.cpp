#include "engine.h"
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
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

    m_xq.do_move(src, dst);
    if (m_xq.is_win())
    {
        m_xq.undo_move(src, dst, dst_piece);
        return false;
    }
    ++m_ply;
    m_traces[m_ply] = create_trace(m_xq.check_status(), dst_piece, move);
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

Engine::Engine(const XQ& xq, uint hash):m_xq(xq), m_ply(0), m_hash(hash)
{
    assert(!xq.is_win());

    m_traces[0] = create_trace(xq.check_status(), EmptyIndex, 0);
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

void Engine::reset(const XQ& xq)
{
    assert(!xq.is_win());

    m_xq = xq;

    m_traces[0] = create_trace(xq.check_status(), EmptyIndex, 0);
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
static vector<uint> generate_root_move(XQ& xq, const set<uint>& ban)
{
    vector<uint> ml = xq.generate_moves();
    for (uint i = 0; i < ml.size(); ++i)
    {
        uint move = ml[i];
        uint dst_piece = xq.square(move_dst(move));
        if (dst_piece == RedKingIndex || dst_piece == BlackKingIndex)
        {
            ml.clear();
            ml.push_back(move);
            return ml;
        }
        bool remove = false;
        if (ban.find(move) != ban.end())
            remove = true;
        else
        {
            xq.do_move(move_src(move), move_dst(move));
            if (xq.is_win())
                remove = true;
            xq.undo_move(move_src(move), move_dst(move), dst_piece);
        }
        if (remove)
        {
            ml.erase(ml.begin()+i);
            --i;
        }
    }
    return ml;
}
uint32 Engine::search(int depth, set<uint> ban)
{
    ofstream file("folium.txt", ios::out|ios::app);
    file << string(m_xq) << endl;
    m_stop = false;
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

    m_null_ply = m_start_ply = m_ply;

    int best_value;
    vector<uint> ml = generate_root_move(m_xq, ban);
    vector<uint> bests;
    clock_t start;
    start = clock();
    for (sint i = 1; 
        i <= depth  || (i <= depth*2 && float(clock()-start)/CLOCKS_PER_SEC < 2.0);
        ++i)
    {
        if (m_stop)
            break;
        if (ml.size() == 1)
        {
            file <<  "unique move!" << endl;
            bests.clear();
            bests.push_back(ml[0]);
            break;
        }
        if (ml.size() == 0)
        {
            file <<  "lost!"<< endl;
            break;
        }
        best_value = -WINSCORE;
        vector<uint> olds;
        bests.swap(olds);
        for (uint j = 0; j < olds.size(); ++j)
        {
            if (m_stop)
                break;
            int score;
            uint32 move = olds[j];
            make_move(move);
            if (best_value != -WINSCORE)
            {
                score = - full(i, -1-best_value, 1-best_value);
                if (score > best_value)
                    score = - full(i, -WINSCORE, -1-best_value);
            }
            else
            {
                score = - full(i, -WINSCORE, WINSCORE);
            }
            unmake_move();
            if (score >= best_value)
            {
                if (score > best_value)
                    bests.clear();
                if (find(bests.begin(), bests.end(), move) == bests.end())
                {
                    bests.push_back(move);
                    file << i << "\t" << move_src(move) << "\t" << move_dst(move) << "\t" << score << endl;
                }
                best_value = score;
            }
        }
        for (uint j = 0; j < ml.size(); ++j)
        {
            if (m_stop)
                break;
            uint32 move = ml[j];
            if (!make_move(move))
            {
                ml[j] = 0;
                continue;
            }
            int score;
            if (best_value != -WINSCORE)
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
            if (score > best_value)
            {
                if (score > best_value)
                    bests.clear();
                if (find(bests.begin(), bests.end(), move) == bests.end())
                {
                    bests.push_back(move);
                    file << i << "\t" << move_src(move) << "\t" << move_dst(move) << "\t" << score << endl;
                }
                best_value = score;
            }
            else if (score < -MATEVALUE)
            {
                ml[j] = 0;
            }
        }

        float t = float(clock()-start)/CLOCKS_PER_SEC;
        if (t > 0.01)
            file << "nps:\t" << int((m_tree_nodes+m_leaf_nodes)/t) << "\ttime:"<<t<<endl;

        if (best_value > MATEVALUE || best_value < -MATEVALUE)
            break;
        ml.erase(remove(ml.begin(), ml.end(), 0), ml.end());
    }
    file << "depth:\t" << depth << endl;
    for (uint i = 0; i < bests.size(); ++i)
    {
        file << "move:\t" << move_src(bests[i]) << "\t" << move_dst(bests[i]) << endl;
    }
    file << m_tree_nodes << "\t"<<m_leaf_nodes << "\t" << m_quiet_nodes << endl;
    file << m_hash_hit_nodes << "\t" << m_hash_move_cuts << "\t" << m_kill_cuts_1 << "\t" << m_kill_cuts_2 << endl;
    file << m_null_nodes << "\t" <<  m_null_cuts << endl;
    file << "-------------------------------------------------------------" << endl;
    file.close();
    if (bests.empty())
        return 0;
    return bests[clock()%bests.size()];
}
