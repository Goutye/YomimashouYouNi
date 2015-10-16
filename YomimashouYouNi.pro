#-------------------------------------------------
#
# Project created by QtCreator 2015-10-15T12:29:04
#
#-------------------------------------------------

QT       += core gui network xml
QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = YomimashouYouNi
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mangadownloader.cpp

HEADERS  += mainwindow.h \
    mangadownloader.h

FORMS    +=
