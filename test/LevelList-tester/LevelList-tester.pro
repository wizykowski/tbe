QT += xml \
    testlib
TARGET = tester
CONFIG += qt warn_on debug
QMAKE_CXXFLAGS += "--std=gnu++11"
TEMPLATE = app
SOURCES += \
    LevelList-tester-main.cpp \
    ../../src/loadsave/LevelList.cpp \
    ../../src/loadsave/LocalString.cpp
HEADERS += \
    ../../src/tbe_global.h \
    ../../src/loadsave/LevelList.h \
    ../../src/loadsave/LocalString.h
INCLUDEPATH += \
	../../src \
	../../src/loadsave \
	../../src/view
