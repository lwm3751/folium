#include <boost/python.hpp>
namespace python = boost::python;
#include "pipe/io.h"
using namespace folium;
void run_script()
{
    python::object entry = python::import("main");

    shared_ptr<io> io = stdio();
    while (!io->readable());
    string cmd = io->readline();
    if (cmd == "ucci")
    {
        entry.attr("ucci")();
    }
    else if (cmd == "perft")
    {
        entry.attr("perft")();
    }
}

extern "C"  void init_xq();
int main(int argc, char *argv[])
{

    Py_Initialize();
    PySys_SetArgv(argc, argv);
    PyImport_AppendInittab("_xq", init_xq);
    if (python::handle_exception(run_script))
        if (PyErr_Occurred())
            PyErr_Print();
    return 0;
}
