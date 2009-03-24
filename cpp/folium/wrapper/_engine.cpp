#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>
#include "../engine.h"
#include "../utility/time.h"

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
    virtual bool readable();
    virtual string readline();
    virtual void writeline(const string& str);
    object m_io;
};
bool EngineWrap::readable()
{
    if (m_io)
        return extract<bool> (m_io.attr("readable")());
    return Engine::readable();
}
string EngineWrap::readline()
{
    if (m_io)
        return extract<string> (m_io.attr("readline")());
    return Engine::readline();
}
void EngineWrap::writeline(const string& str)
{
    if (m_io)
        m_io.attr("writeline")(str);
    else
        Engine::writeline(str);
}

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
        .def_readwrite("io", &EngineWrap::m_io)
    ;

    def("now", &now_time);
}
