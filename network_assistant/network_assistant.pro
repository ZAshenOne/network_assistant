#-------------------------------------------------
#
# Project created by QtCreator 2023-09-06T20:37:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = network_assistant
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    net.cpp

HEADERS  += mainwindow.h \
    net.h

FORMS    += mainwindow.ui

RESOURCES += \
    picture/picture.qrc

DISTFILES +=
