TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

DESTDIR = $$PWD/../../bin/tests/

#Отключить "теневую сборку" в криейторе!
CONFIG(release, debug|release) {

message(Project $$TARGET (Release))

OBJECTS_DIR = build/
MOC_DIR = build/
RCC_DIR = build/
UI_DIR = build/
}
CONFIG(debug, debug|release) {

message(Project $$TARGET (Debug))

OBJECTS_DIR = build/
MOC_DIR = build/
RCC_DIR = build/
UI_DIR = build/
DEFINES += DEBUG_BUILD
}

SOURCES += \
        UtilsTest.cpp

# Доп. файлы для тестирования
INCLUDEPATH += ../../include
HEADERS += \
    ../../include/Logger.h \
    ../../include/TradingCompany.h \
    ../../include/Utils.h

SOURCES += \
    ../../src/Logger.cpp \
    ../../src/TradingCompany.cpp \
    ../../src/Utils.cpp

# Boost
LIBS += -L/usr/lib/x86_64-linux-gnu \
        -lboost_filesystem \
        -lboost_system \
        -lboost_regex \
        -lboost_unit_test_framework \
        -lpthread
