#include "engine.h"

#include <fstream>
using namespace std;

bool Engine::make_move(uint32 move)
{
    register uint src, dst, src_piece, dst_piece;
    register uint own;
    register uint op, np;
    src = move_src(move);
    dst = move_dst(move);
    assert(m_xq.is_legal_move(src, dst));
    src_piece = m_xq.square(src);
    dst_piece = m_xq.square(dst);

    own = m_xq.player();
    m_xq.m_bitlines.changebit(square_x(src), square_y(src));
    m_xq.m_bitlines.setbit(square_x(dst), square_y(dst));
    m_xq.m_squares[src] = EmptyIndex;
    m_xq.m_squares[dst] = src_piece;
    m_xq.m_pieces[src_piece] = dst;
    m_xq.m_pieces[dst_piece] = InvaildSquare;
    if (m_xq.in_checked(own))
    {
        m_xq.m_bitlines.setbit(square_x(src), square_y(src));
        if (dst_piece == EmptyIndex)
            m_xq.m_bitlines.changebit(square_x(dst), square_y(dst));
        m_xq.m_squares[src] = src_piece;
        m_xq.m_squares[dst] = dst_piece;
        m_xq.m_pieces[src_piece] = src;
        m_xq.m_pieces[dst_piece] = dst;
        return false;
    }
    m_xq.m_player = 1UL - own;

    op = m_ply;
    np = op + 1;

    m_traces[np] = create_trace(m_xq.in_checked(1 - own), dst_piece, move);
    if (dst_piece == EmptyIndex)
    {
        m_keys[np] = m_keys[op]\
                    ^ piece_key(src_piece, src)
                    ^ piece_key(src_piece, dst);
        m_locks[np] = m_locks[op]\
                    ^ piece_lock(src_piece, src)\
                    ^ piece_lock(src_piece, dst);;
        m_values[np] = m_values[op]\
                        + piece_value(src_piece, dst)\
                        - piece_value(src_piece, src);
    }
    else
    {
        m_keys[np] = m_keys[op]\
                    ^ piece_key(src_piece, src)\
                    ^ piece_key(src_piece, dst)\
                    ^ piece_key(dst_piece, dst);
        m_locks[np] = m_locks[op]\
                    ^ piece_lock(src_piece, src)\
                    ^ piece_lock(src_piece, dst)\
                    ^ piece_lock(dst_piece, dst);
        m_values[np] = m_values[op]\
                        + piece_value(src_piece, dst)\
                        - piece_value(src_piece, src)\
                        - piece_value(dst_piece, dst);
    }
    m_ply = np;
    return true;
}

void Engine::unmake_move()
{
    assert (m_ply > 0);
    register uint src, dst, src_piece, dst_piece;
    register uint32 trace = m_traces[m_ply--];
    src = trace_src(trace);
    dst = trace_dst(trace);
    dst_piece = trace_dst_piece(trace);
    src_piece = m_xq.square(dst);

    if (dst_piece == EmptyIndex)
    {
        m_xq.m_bitlines.changebit(square_x(dst), square_y(dst));
    }
    m_xq.m_bitlines.changebit(square_x(src), square_y(src));
    m_xq.m_squares[src] = src_piece;
    m_xq.m_squares[dst] = dst_piece;
    m_xq.m_pieces[src_piece] = src;
    m_xq.m_pieces[dst_piece] = dst;
    m_xq.m_player = 1UL - m_xq.m_player;
}

Engine::Engine(const XQ& xq, uint hash):m_xq(xq), m_ply(0), m_hash(hash)
{
    assert(!xq.in_checked(1 - xq.player()));

    m_traces[0] = create_trace(xq.in_checked(xq.player()), EmptyIndex, 0);
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
    assert(!xq.in_checked(1 - xq.player()));

    m_xq = xq;

    m_traces[0] = create_trace(xq.in_checked(xq.player()), EmptyIndex, 0);
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
void Engine::generate_root_move(MoveList& movelist, MoveList& ban)
{
    movelist.clear();
    MoveList _;
    m_xq.generate_moves(_);
    for (uint i = 0; i < _.size(); ++i)
    {
        uint move = _[i] & 0x3fff;
        uint piece = m_xq.square(move_dst(move));
        if (piece == RedKingIndex || piece == BlackKingIndex)
        {
            movelist.clear();
            movelist.push(move_src(move), move_dst(move));
            return;
        }
        bool add = true;
        for (uint j = 0; j < ban.size(); ++j)
        {
            if (move == (ban[j] & 0x3fff))
            {
                add = false;
                break;
            }
        }
        if (!make_move(move))
            continue;
        unmake_move();
        if (add)
            movelist.push(move_src(move), move_dst(move));
    }
}
uint32 Engine::search(int depth, MoveList& ban)
{
    ofstream file("folium.txt", ios::out|ios::app);
    file << string(m_xq) << endl;
    m_stop = false;
    m_tree_nodes = 0;
    m_quiet_nodes = 0;
    m_hash_hit_nodes = 0;
    m_hash_move_cuts = 0;
    m_null_nodes = 0;
    m_null_cuts = 0;

    m_history.clear();
    m_hash.clear();

    m_null_ply = m_start_ply = m_ply;

    uint32 best_move = 0;
    MoveList ml;
    generate_root_move(ml, ban);
    if (ml.size() == 1)
    {
        file <<  "unique move:\t "<< move_src(ml[0]) << "\t" << move_dst(ml[0]) << endl;
        file << "-------------------------------------------------------------" << endl;
        return ml[0];
    }
    if (ml.size() == 0)
        return 0;

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
            file << depth << ":\t" << move_src(best_move) << "\t" << move_dst(best_move) << endl;
            file << m_tree_nodes << "\t" << m_quiet_nodes << "\t" << m_hash_hit_nodes << "\t" << m_hash_move_cuts<< endl;
            file << "-------------------------------------------------------------" << endl;
            return best_move;
        }
    }
    file << "depth:\t" << depth << endl << "move:\t" << move_src(best_move) << "\t" << move_dst(best_move) << endl;
    file << m_tree_nodes << "\t" << m_quiet_nodes << "\t" << m_hash_hit_nodes << "\t" << m_hash_move_cuts << "\t" << m_null_nodes << "\t" <<  m_null_cuts << endl;
    file << "-------------------------------------------------------------" << endl;
    file.close();
    return best_move;
}
