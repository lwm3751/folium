#include "engine.h"
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

bool Engine::make_move(uint32 move)
{
    uint src, dst, src_piece, dst_piece;
    src = move_src(move);
    dst = move_dst(move);
    assert(m_xq.is_legal_move(src, dst));
    src_piece = m_xq.square(src);
    dst_piece = m_xq.square(dst);
    assert(dst_piece != RedKingIndex && dst_piece != RedKingIndex);

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

    uint32 best_move = 0;
    vector<uint> ml = generate_root_move(m_xq, ban);
    if (ml.size() == 1)
    {
        file <<  "unique move:\t "<< move_src(ml[0]) << "\t" << move_dst(ml[0]) << endl;
        file << "-------------------------------------------------------------" << endl;
        return ml[0];
    }
    if (ml.size() == 0)
    {
        file <<  "not move!"<< endl;
        file << "-------------------------------------------------------------" << endl;
        return 0;
    }
    clock_t start;
    start = clock();
    for (sint i = 1; i <= depth; ++i)
    {
        bool found = false;
        int best_value = -WINSCORE;
        int alpha = -WINSCORE;
        if (best_move)
        {
            assert(m_xq.is_legal_move(move_src(best_move), move_dst(best_move)));
            make_move(best_move);
            best_value = - full(i, -WINSCORE, -alpha);
            unmake_move();
            file << i << "\t" << move_src(best_move) << "\t" << move_dst(best_move) << "\t" << best_value << endl;
            if (m_stop)
                return best_move;
            alpha = best_value;
            found = true;
        }
        for (uint j = 0; j < ml.size(); ++j)
        {
            uint32 move = ml[j];
            assert(m_xq.is_legal_move(move_src(move), move_dst(move)));
            if (move == best_move)
                continue;
            make_move(move);
            int score;
            if (found)
            {
                score = - mini(i, -alpha);
                if (score > alpha)
                    score = - full(i, -WINSCORE, -alpha);
            }
            else
            {
                score = - full(i, -WINSCORE, -alpha);
            }
            //int score = - alpha_beta(i, -WINSCORE, -alpha);
            unmake_move();
            if (m_stop)
                return best_move;
            if (score > best_value)
            {
                file << i << "\t" << move_src(move) << "\t" << move_dst(move) << "\t" << score << endl;
                best_value = score;
                best_move = move;
                if (score > alpha)
                {
                    found = true;
                    alpha = score;
                }
            }
        }
        if (best_value > MATEVALUE || best_value < -MATEVALUE)
        {
            float t = float(clock()-start)/CLOCKS_PER_SEC;
            file << "nps1:\t" << int((m_tree_nodes+m_leaf_nodes+m_quiet_nodes)/t) << "\ttime:"<<t<<endl;
            file << "nps2:\t" << int((m_tree_nodes+m_leaf_nodes)/t) << "\ttime:"<<t<<endl;
            file << depth << ":\t" << move_src(best_move) << "\t" << move_dst(best_move) << endl;
            file << m_tree_nodes << "\t"<<m_leaf_nodes << "\t" << m_quiet_nodes << endl;
            file << m_hash_hit_nodes << "\t" << m_hash_move_cuts << "\t" << m_kill_cuts_1 << "\t" << m_kill_cuts_2 << endl;
            file << m_null_nodes << "\t" <<  m_null_cuts << endl;
            file << "-------------------------------------------------------------" << endl;
            return best_move;
        }
    }
    float t = float(clock()-start)/CLOCKS_PER_SEC;
    file << "nps1:\t" << int((m_tree_nodes+m_leaf_nodes+m_quiet_nodes)/t) << "\ttime:"<<t<<endl;
    file << "nps2:\t" << int((m_tree_nodes+m_leaf_nodes)/t) << "\ttime:"<<t<<endl;
    file << "depth:\t" << depth << endl << "move:\t" << move_src(best_move) << "\t" << move_dst(best_move) << endl;
    file << m_tree_nodes << "\t"<<m_leaf_nodes << "\t" << m_quiet_nodes << endl;
    file << m_hash_hit_nodes << "\t" << m_hash_move_cuts << "\t" << m_kill_cuts_1 << "\t" << m_kill_cuts_2 << endl;
    file << m_null_nodes << "\t" <<  m_null_cuts << endl;
    file << "-------------------------------------------------------------" << endl;
    file.close();
    return best_move;
}
