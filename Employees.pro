TARGET = Employees
QT += core gui
QT += sql
QT += core network # Для работы с сетевыми классами
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++20
CONFIG -= app_bundle

# Build properties

DESTDIR = $$PWD/bin

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

macx
{
    INCLUDEPATH = /opt/homebrew/include
    COMMON_LIBS_DIR = /opt/homebrew/lib
}

INCLUDEPATH += $$PWD/include
HEADERS += \
    include/client.h \
    include/database.h \
    include/dialog.h \
    include/employee.h \
    include/server.h \
    include/user.h \
    include/utils.h \
    include/table.h

#HEADERS += \
#/opt/homebrew/lib

SOURCES += \
    src/client.cpp \
    src/database.cpp \
    src/employee.cpp \
    src/main.cpp \
    src/dialog.cpp \
    src/server.cpp \
    src/user.cpp \
    src/utils.cpp \
    src/table.cpp

FORMS += \
    forms/dialog.ui \
    forms/table.ui

# PostgreSql
LIBS += -L$$COMMON_LIBS_DIR \
        -lpq \
        -lpqxx

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    database/employees.sql

RESOURCES += \
    resources.qrc
