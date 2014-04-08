#-------------------------------------------------
#
# Project created by QtCreator 2014-03-04T17:53:17
#
#-------------------------------------------------

QT       += core serialport

QT       -= gui

TARGET = com-mitm
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    cardreader.cpp \
    codereader.cpp


HEADERS += \
    cardreader.h \
    codereader.h
