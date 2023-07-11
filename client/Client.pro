TARGET = Employees
QT += core gui
QT += sql
QT += core network # Для работы с сетевыми классами
QT += widgets
QT += core5compat
CONFIG += c++20
CONFIG -= app_bundle
#CONFIG += debug

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
    include/cache.h \
    include/delegate.h \
    include/quint64validator.h \
    include/tableView.h \
    include/client.h \
    include/dialog.h \
    include/qjsontablemodel.h \
    include/table_p.h \
    include/table.h \
    include/utils.h

#HEADERS += \
#/opt/homebrew/lib

SOURCES += \
    src/cache.cpp \
    src/delegate.cpp \
    src/quint64validator.cpp \
    src/tableView.cpp \
    src/client.cpp \
    src/main.cpp \
    src/dialog.cpp \
    src/qjsontablemodel.cpp \
    src/table_p.cpp \
    src/table.cpp \
    src/utils.cpp

FORMS += \
    forms/dialog.ui \
    forms/table.ui

# PostgreSql
LIBS += -L$$COMMON_LIBS_DIR \
        -lpq \
        -lpqxx

include(requester/requester.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


RESOURCES += \
    resources.qrc
