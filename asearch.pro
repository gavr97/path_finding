TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_LFLAGS += -static

SOURCES += main.cpp \
    algorithm/theta.cpp \
    algorithm/astar.cpp \
    algorithm/node.cpp \
    dataCode/output.cpp \
    dataCode/task.cpp \
    dataCode/map.cpp \
    logger/log.cpp \
    tinyxml/tinyxml2.cpp \
    algorithm/close.cpp \
    algorithm/open.cpp \
    algorithm/jps.cpp \
    algorithm/heuristics.cpp \
    algorithm/coordinats.cpp

DISTFILES += \
    algorithm/astar.h.gch \
    dataCode/task.h.gch \
    logger/log.h.gch \
    tinyxml/tinyxml2.h.gch

HEADERS += \
    algorithm/theta.h \
    algorithm/astar.h \
    algorithm/node.h \
    dataCode/output.h \
    dataCode/task.h \
    dataCode/map.h \
    global/globalTypes.h \
    global/globalVars.h \
    logger/log.h \
    tinyxml/tinyxml2.h \
    algorithm/close.h \
    algorithm/open.h \
    algorithm/jps.h \
    algorithm/heuristics.h \
    algorithm/coordinats.h
