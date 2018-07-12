TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    include/sharedmemory.cpp

HEADERS += \
    include/sharedmemory.hpp
