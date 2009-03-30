TEMPLATE = lib
TARGET = folium
CONFIG += dll
CONFIG -= qt

HEADERS += bitmap.h
HEADERS += defines.h
HEADERS += engine.h
HEADERS += generator.h
HEADERS += hash.h
HEADERS += history.h
HEADERS += int.h
HEADERS += killer.h
HEADERS += move.h
HEADERS += movelist.h
HEADERS += xq.h
HEADERS += xq_data.h
HEADERS += xq_position_data.h
HEADERS += utility\time.h
HEADERS += utility\str.h

SOURCES += engine.cpp
SOURCES += generator.cpp
SOURCES += hash.cpp
SOURCES += move_helper.cpp
SOURCES += search.cpp
SOURCES += xq.cpp
SOURCES += xq_helper.cpp
SOURCES += bitmap_data.cpp
SOURCES += history_data.cpp
SOURCES += xq_data.cpp
SOURCES += xq_position_data.cpp
SOURCES += utility\str.cpp
SOURCES += utility\time.cpp
SOURCES += wrapper\_engine.cpp
SOURCES += wrapper\_xq.cpp

release {
    OBJECTS_DIR = ../objects/release/folium
}
debug {
    OBJECTS_DIR = ../objects/debug/folium
}

win32 {
    INCLUDEPATH = d:\python26\include D:\toolbox\boost_1_38_0
    DESTDIR = ..
    LIBS += D:\Python26\libs\libpython26.a
    LIBS += D:\toolbox\boost_1_38_0\lib_x86\lib\boost_python-mgw34-mt.lib
}
