#-------------------------------------------------
#
# Project created by QtCreator 2013-02-27T18:52:50
#
#-------------------------------------------------

QT       += core gui

TARGET = evaluator_widget
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp\
        mainwindow.cpp \
    processing.cpp \
    functioncall.cpp

HEADERS  += mainwindow.h \
    processing.h \
    functioncall.h \
    commons.h

FORMS    += mainwindow.ui
