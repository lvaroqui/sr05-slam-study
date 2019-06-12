TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    pathfinder.cpp

HEADERS += \
    pathfinder.h \
    ../Monitor/utils.h
