#-------------------------------------------------
#
# Project created by QtCreator 2014-10-12T14:41:26
#
#-------------------------------------------------

QT       += core gui network xml multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = somafm-qt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dataprovider.cpp \
    channelsview.cpp \
    playerview.cpp

HEADERS  += mainwindow.h \
    dataprovider.h \
    channelsview.h \
    playerview.h
