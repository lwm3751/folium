#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>
#include "../engine.h"
#include "../utility/time.h"
#include "../pipe/io.h"

using namespace std;
using namespace boost;
using namespace boost::python;
using namespace folium;
class EngineWrap : public Engine
{
public:
    EngineWrap():
        m_io(stdio()),
        m_depth(8),
        m_starttime(0.0f),
        m_mintime(0.0f),
        m_maxtime(0.0f),
        m_ponder(false)
    {
    }
    folium::uint32 search(list bans)
    {
        set<folium::uint> s;
        return Engine::search(s);
    }
    virtual bool readable();
    virtual string readline();
    virtual void writeline(const string& str);
    virtual void interrupt();
    virtual bool searchable(sint depth);
private:
    shared_ptr<io> m_io;
public:
    int m_depth;
    double m_starttime;
    double m_mintime;
    double m_maxtime;
    bool m_ponder;
};
bool EngineWrap::readable()
{
    if (m_io)
        return m_io->readable();
    return false;
}

string EngineWrap::readline()
{
    if (m_io)
        return m_io->readline();
    return string();
}

void EngineWrap::writeline(const string& str)
{
    if (m_io)
        m_io->writeline(str);
}

void EngineWrap::interrupt()
{
    if (!m_ponder && current_time() >= m_maxtime)
        m_stop = true;
    if (!m_stop && readable())
    {
        string line = readline();
        if (line == "isready")
            writeline("readyok");
        else if (line == "stop")
            m_stop = true;
        else if (line == "ponderhit")
            m_ponder = false;
    }
}

bool EngineWrap::searchable(sint depth)
{
    return !m_stop  && depth < m_depth && current_time() < m_mintime;
}

void _init_engine()
{
    //Engine
    class_<EngineWrap, noncopyable>("Engine")
        .def("readable", &EngineWrap::readable)
        .def("readline", &EngineWrap::readline)
        .def("writeline", &Engine::writeline)

        .def("setxq", &Engine::setxq)
        .def("load", &Engine::load)
        .def("__str__", &Engine::fen)

        .def("makemove", &Engine::make_move)
        .def("unmakemove", &Engine::unmake_move)

        .def("search", &EngineWrap::search)

        .def_readwrite("debug", &Engine::m_debug)
        .def_readwrite("stop", &Engine::m_stop)
        .def_readwrite("ponder", &EngineWrap::m_ponder)
        .def_readwrite("depth", &EngineWrap::m_depth)
        .def_readwrite("starttime", &EngineWrap::m_starttime)
        .def_readwrite("mintime", &EngineWrap::m_mintime)
        .def_readwrite("maxtime", &EngineWrap::m_maxtime)
    ;

    def("currenttime", &current_time);
}
