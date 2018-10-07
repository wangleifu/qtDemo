#-------------------------------------------------
#
# Project created by QtCreator 2018-07-09T15:21:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gui-test
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    trace.cpp \
    flight_controller.cpp \
    DBHelper.cpp

HEADERS  += widget.h \
    trace.hpp \
    hust.hpp \
    flight_controller.hpp \
    DBHelper.hpp

FORMS    += widget.ui
