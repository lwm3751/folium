#include <boost/python.hpp>
namespace python = boost::python;

void run_script()
{
    python::object entry = python::import("pyfolium.entry");
    entry.attr("main")();
}

extern "C"  void initfolium();
int main(int argc, char *argv[])
{
    Py_Initialize();
    PySys_SetArgv(argc, argv);
    PyImport_AppendInittab("folium", initfolium);
    if (python::handle_exception(run_script))
        if (PyErr_Occurred())
            PyErr_Print();
    return 0;
}
