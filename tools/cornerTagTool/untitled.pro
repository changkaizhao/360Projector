#-------------------------------------------------
#
# Project created by QtCreator 2018-05-23T11:18:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dlightgraphicsview.cpp \
    dlightgraphicsscene.cpp \
    dlightgraphicsellipseitem.cpp

HEADERS  += mainwindow.h \
    dlightgraphicsview.h \
    dlightgraphicsscene.h \
    dlightgraphicsellipseitem.h \
    common.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
