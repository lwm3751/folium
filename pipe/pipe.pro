TEMPLATE = lib
TARGET = pipe
CONFIG += dll
CONFIG -= qt
DEFINES -= UNICODE

HEADERS = io.h eleeye\base.h eleeye\base2.h eleeye\pipe.h
SOURCES = io.cpp io_wrapper.cpp eleeye\pipe.cpp

release {
    OBJECTS_DIR = ../objects/release/pipe
}
debug {
    OBJECTS_DIR = ../objects/debug/pipe
}

win32 {
    INCLUDEPATH = d:\python26\include D:\toolbox\boost_1_38_0
    DESTDIR = ..
    LIBS += D:\Python26\libs\libpython26.a
    LIBS += D:\toolbox\boost_1_38_0\lib_x86\lib\boost_python-mgw34-mt.lib
}
