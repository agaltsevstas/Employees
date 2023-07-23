TARGET = Employees
QT += core gui
QT += sql
QT += core network # Для работы с сетевыми классами
QT += widgets
QT += core5compat
QT += httpserver
CONFIG += c++20
CONFIG -= app_bundle

# Build properties

DESTDIR = $$PWD/bin

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
}

macx
{
    INCLUDEPATH = /opt/homebrew/include
    COMMON_LIBS_DIR = /opt/homebrew/lib
}

INCLUDEPATH += $$PWD/include
HEADERS += \
    include/authenticationservice.h \
    include/httpserver.h \
    include/database.h \
    include/logger.h \
    include/server.h \
    include/utils.h

SOURCES += \
    src/authenticationservice.cpp \
    src/database.cpp \
    src/httpserver.cpp \
    src/logger.cpp \
    src/main.cpp \
    src/server.cpp \
    src/utils.cpp

# PostgreSql
LIBS += -L$$COMMON_LIBS_DIR \
        -lpq \
        -lpqxx

include(qjsonwebtoken/qjsonwebtoken.pri)


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    database/employees.sql
