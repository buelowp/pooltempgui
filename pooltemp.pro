TARGET = pooltemp
TEMPLATE = app
CONFIG += debug
QT += gui network widgets qmqtt

OBJECTS_DIR = .obj
MOC_DIR = .moc

SOURCES = \
    main.cpp \
    pooltemp.cpp

HEADERS = \
    pooltemp.h

