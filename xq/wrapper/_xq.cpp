#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>
#include "../xq.h"
#include "../move_helper.h"

using namespace std;
using namespace boost;
using namespace boost::python;
using namespace folium;

shared_ptr<XQ> XQ__init__(const string& fen)
{
    shared_ptr<XQ> xq(new XQ());
    if (xq->set_fen(fen))
    {
        return xq;
    }
    return shared_ptr<XQ>();
}

bool XQ_is_legal_move(XQ& xq, uint32 move)
{
    if (xq.coordinate_color(move_src(move)) != xq.player())
        return false;
    if (!is_legal_move(xq, move_src(move), move_dst(move)))
        return false;
    uint dst_piece = xq.coordinate(move_dst(move));
    if (!xq.do_move(move_src(move), move_dst(move)))
        return false;
    xq.undo_move(move_src(move), move_dst(move), dst_piece);
    return true;
}

void _init_xq()
{
    //XQ
    class_<XQ, shared_ptr<XQ>, noncopyable>("XQ", no_init)
        .def("is_legal_move", &XQ_is_legal_move)
        .def("player_in_check", &player_in_check)

        .def("player", &XQ::player)

        .def("piece", &XQ::piece)

        .def("coordinate", &XQ::coordinate)
        .def("coordinate_color", &XQ::coordinate_color)
        .def("coordinate_is_empty", &XQ::coordinate_is_empty)

        .def("make_move", &XQ::do_move)
        .def("unmake_move", &XQ::undo_move)

        .def("__str__", &XQ::get_fen)
    ;
    def("XQ", &XQ__init__);
}


extern void _init_engine();
BOOST_PYTHON_MODULE(_xq)
{
    _init_xq();
    _init_engine();

    //piece type
    scope().attr("color_red") = Red;
    scope().attr("color_black") = Black;
    scope().attr("color_none") = Empty;
    scope().attr("color_invalid") = Invaild;

    scope().attr("redking") = RedKing;
    scope().attr("redadvisor") = RedAdvisor;
    scope().attr("redbishop") = RedBishop;
    scope().attr("redrook") = RedRook;
    scope().attr("redknight") = RedKnight;
    scope().attr("redcannon") = RedCannon;
    scope().attr("redpawn") = RedPawn;

    scope().attr("blackking") = BlackKing;
    scope().attr("blackadvisor") = BlackAdvisor;
    scope().attr("blackbishop") = BlackBishop;
    scope().attr("blackrook") = BlackRook;
    scope().attr("blackknight") = BlackKnight;
    scope().attr("blackcannon") = BlackCannon;
    scope().attr("blackpawn") = BlackPawn;

    scope().attr("emptypiece") = EmptyPiece;
    scope().attr("invaildpiece") = InvaildPiece;

    //piece flag
    scope().attr("redkingflag") = RedKingFlag;
    scope().attr("redadvisorflag") = RedAdvisorFlag;
    scope().attr("redbishopflag") = RedBishopFlag;
    scope().attr("redrookflag") = RedRookFlag;
    scope().attr("redknightflag") = RedKnightFlag;
    scope().attr("redcannonflag") = RedCannonFlag;
    scope().attr("redpawnflag") = RedPawnFlag;

    scope().attr("blackkingflag") = BlackKingFlag;
    scope().attr("blackadvisorflag") = BlackAdvisorFlag;
    scope().attr("blackbishopflag") = BlackBishopFlag;
    scope().attr("blackrookflag") = BlackRookFlag;
    scope().attr("blackknightflag") = BlackKnightFlag;
    scope().attr("blackcannonflag") = BlackCannonFlag;
    scope().attr("blackpawnflag") = BlackPawnFlag;

    scope().attr("emptypieceflag") = EmptyFlag;
    scope().attr("invaildpieceflag") = InvaildFlag;

    def("piece_type", &folium::piece_type);
    def("piece_color", &folium::piece_color);

    //coordinate
    scope().attr("invaild_coordinate") = InvaildCoordinate;

    def("coordinate_x", &folium::coordinate_x);
    def("coordinate_y", &folium::coordinate_y);
    def("xy_coordinate", &folium::xy_coordinate);

    def("move_src", &folium::move_src);
    def("move_dst", &folium::move_dst);
    def("create_move", &folium::create_move);

    def("ucci2move", &folium::ucci2move);
    def("move2ucci", &folium::move2ucci);

    def("mirror4uccimove", &folium::mirror4uccimove);
    def("mirror4fen", &folium::mirror4fen);
}
