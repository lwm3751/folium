#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>
using namespace boost;
using namespace boost::python;

#include "io.h"
using namespace folium;

BOOST_PYTHON_MODULE(pipe)
{
    class_<io, shared_ptr<io>, noncopyable>("Pipe", no_init)
        .def("readable", &io::readable)
        .def("writeline", &io::writeline)
        .def("readline", &io::readline)
    ;
    def("openexe", &openexe);
    scope().attr("stdio") = stdio();
}
