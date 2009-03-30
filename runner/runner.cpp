#include <boost/python.hpp>
namespace python = boost::python;

void run_script()
{
    python::object entry = python::import("pyfolium.entry");
    entry.attr("main")();
}

int main(int argc, char *argv[])
{
    Py_Initialize();
    PySys_SetArgv(argc, argv);
    if (python::handle_exception(run_script))
        if (PyErr_Occurred())
            PyErr_Print();
    return 0;
}
