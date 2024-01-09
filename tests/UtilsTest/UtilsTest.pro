TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

DESTDIR = $$PWD/../../bin/tests/

#Отключить "теневую сборку" в криейторе!
CONFIG(release, debug|release)
{
    message(Project $$TARGET (Release))

    OBJECTS_DIR = build/
    MOC_DIR = build/
    RCC_DIR = build/
    UI_DIR = build/
}
CONFIG(debug, debug|release)
{
    message(Project $$TARGET (Debug))

    OBJECTS_DIR = build/
    MOC_DIR = build/
    RCC_DIR = build/
    UI_DIR = build/
    DEFINES += DEBUG_BUILD

    # Boost
    LIBS += -L/usr/lib/x86_64-linux-gnu \
    -lboost_filesystem \
    -lboost_system \
    -lboost_program_options \
    -lboost_regex \
    -lpthread
}

macx
{
    INCLUDEPATH = /opt/homebrew/include
    COMMON_LIBS_DIR = /opt/homebrew/lib

    # Boost
    LIBS += -L$$COMMON_LIBS_DIR \
            -lboost_filesystem \
            -lboost_system \
            -lboost_program_options \
            -lboost_regex \
            -lpthread
}

SOURCES += \
        UtilsTest.cpp

# Доп. файлы для тестирования
INCLUDEPATH += ../../include
HEADERS += \
    ../../include/Logger.h \
    ../../include/IEmployee.h \
    ../../include/Employee.h \
    ../../include/Utils.h

SOURCES += \
    ../../src/Logger.cpp \
    ../../src/Employee.cpp \
    ../../src/Utils.cpp
