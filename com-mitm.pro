#-------------------------------------------------
#
# Project created by QtCreator 2014-03-04T17:53:17
#
#-------------------------------------------------

QT       += core serialport

QT       -= gui

TARGET = com-mitm
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    cardreader.cpp


QMAKE_CXXFLAGS += --std=c++11

HEADERS += \
    cardreader.h
