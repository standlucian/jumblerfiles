# -------------------------------------------------
# Project created by QtCreator 2009-07-14T17:06:02
# -------------------------------------------------
TARGET = jumbler
TEMPLATE = app
CONFIG += qt
QT += network widgets
CONFIG += thread
QMAKE_CXXFLAGS_RELEASE += -g -march=native\
    -O3
LIBS += -g \
    -lboost_filesystem \
    -lboost_system 
INCLUDEPATH += include
SOURCES += sources/main.cpp \
    sources/mainwindow.cpp
HEADERS += include/main.h \
    include/mainwindow.h
#OTHER_FILES +=

