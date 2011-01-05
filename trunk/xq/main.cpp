#include <boost/python.hpp>
namespace python = boost::python;

void run_script()
{
    python::object entry = python::import("main");
    entry.attr("main")();
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
