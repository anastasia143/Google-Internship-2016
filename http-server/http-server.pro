#-------------------------------------------------
#
# Project created by QtCreator 2016-02-16T12:23:52
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = http-server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


DEFINES += BOOST_THREAD_USE_LIB
CONFIG += static

SOURCES += main.cpp \
    server.cpp

INCLUDEPATH += "C:/boost_1_60_0"

# boost libs
LIBS += "-LC:/boost_1_60_0/stage/lib/"
LIBS += "C:/boost_1_60_0/stage/lib/libboost_system-mgw49-mt-1_60.a"
LIBS += "C:/boost_1_60_0/stage/lib/libboost_system-mgw49-mt-d-1_60.a"
LIBS += "C:/boost_1_60_0/stage/lib/libboost_thread-mgw49-mt-1_60.a"
LIBS += "C:/boost_1_60_0/stage/lib/libboost_thread-mgw49-mt-d-1_60.a"
LIBS += "C:/boost_1_60_0/stage/lib/libboost_date_time-mgw49-mt-1_60.a"
LIBS += "C:/boost_1_60_0/stage/lib/libboost_date_time-mgw49-mt-d-1_60.a"
LIBS += "C:/boost_1_60_0/stage/lib/libboost_regex-mgw49-mt-1_60.a"
LIBS += "C:/boost_1_60_0/stage/lib/libboost_regex-mgw49-mt-d-1_60.a"
LIBS += "C:/boost_1_60_0/stage/lib/libboost_serialization-mgw49-mt-1_60.a"
LIBS += "C:/boost_1_60_0/stage/lib/libboost_serialization-mgw49-mt-d-1_60.a"

# windows libs for sockets
win32:LIBS += -lsetupapi
win32:LIBS += -lwsock32
win32:LIBS += -lws2_32
win32:DEFINES += _WIN32_WINNT=0x0501

HEADERS += \
    server.h

