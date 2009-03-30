TEMPLATE = app
TARGET = runner
CONFIG += console
CONFIG -= qt
SOURCES += runner.cpp
release {
    OBJECTS_DIR = ../objects/release/runner
}
debug {
    OBJECTS_DIR = ../objects/debug/runner
}
win32 {
    INCLUDEPATH = d:\python26\include D:\toolbox\boost_1_38_0
    DESTDIR = ..
    LIBS += D:\Python26\libs\libpython26.a
    LIBS += D:\toolbox\boost_1_38_0\lib_x86\lib\boost_python-mgw34-mt.lib
}