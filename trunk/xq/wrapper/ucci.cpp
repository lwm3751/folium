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
class UCCI : public Engine
{
public:
    UCCI():
        m_io(),
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

inline bool _readable()
{
    return stdio()->readable();
}

inline string _readline()
{
    return stdio()->readline();
}

inline void _writeline(const string& str)
{
    stdio()->writeline(str);
}

void UCCI::writeline(const string& str)
{
    stdio()->writeline(str);
}

void UCCI::interrupt()
{
    if (!m_ponder && current_time() >= m_maxtime)
        m_stop = true;
    if (!m_stop && _readable())
    {
        string line = _readline();
        if (line == "isready")
            _writeline("readyok");
        else if (line == "stop")
            m_stop = true;
        else if (line == "ponderhit")
            m_ponder = false;
    }
}

bool UCCI::searchable(sint depth)
{
    return !m_stop  && depth < m_depth && current_time() < m_mintime;
}

void _init_engine()
{
    //Engine
    class_<UCCI, noncopyable>("UCCI")
        .def("setxq", &Engine::setxq)
        .def("load", &Engine::load)
        .def("__str__", &Engine::fen)

        .def("makemove", &Engine::make_move)
        .def("unmakemove", &Engine::unmake_move)

        .def("search", &UCCI::search)

        .def_readwrite("debug", &Engine::m_debug)
        .def_readwrite("stop", &Engine::m_stop)
        .def_readwrite("ponder", &UCCI::m_ponder)
        .def_readwrite("depth", &UCCI::m_depth)
        .def_readwrite("starttime", &UCCI::m_starttime)
        .def_readwrite("mintime", &UCCI::m_mintime)
        .def_readwrite("maxtime", &UCCI::m_maxtime)
    ;

    def("readable", &_readable);
    def("readline", &_readline);
    def("writeline", &_writeline);
    def("currenttime", &current_time);
}
