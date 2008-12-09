#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>
#include "../folium/engine.h"

using namespace std;
using namespace boost;
using namespace boost::python;
using namespace folium;
class EngineWrap : public Engine
{
public:
    folium::uint32 search(list bans)
    {
        set<folium::uint> s;
        return Engine::search(s);
    }
};

void init_engine()
{
    //Engine
    class_<EngineWrap, noncopyable>("Engine")
        .def("readable", &Engine::readable)
        .def("readline", &Engine::readline)
        .def("writeline", &Engine::writeline)

        .def("setxq", &Engine::setxq)
        .def("load", &Engine::load)
        .def("__str__", &Engine::fen)

        .def("makemove", &Engine::make_move)
        .def("unmakemove", &Engine::unmake_move)

        .def("search", &EngineWrap::search)

        .def_readwrite("debug", &Engine::m_debug)
        .def_readwrite("stop", &Engine::m_stop)
        .def_readwrite("ponder", &Engine::m_ponder)
        .def_readwrite("depth", &Engine::m_depth)
        .def_readwrite("starttime", &Engine::m_starttime)
        .def_readwrite("mintime", &Engine::m_mintime)
        .def_readwrite("maxtime", &Engine::m_maxtime)
        .def_readonly("pipe", &Engine::m_io)
    ;

    def("time", &now_real);
}
