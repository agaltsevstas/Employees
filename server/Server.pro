TARGET = Employees
QT += core gui
QT += sql
QT += core network # Для работы с сетевыми классами
QT += widgets
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
    include/controller/authenticationservice.h \
    include/database.h \
    include/employee.h \
    include/utils.h \
    include/controller/authorizationcontroller.h \
    include/controller/requestmapper.h

SOURCES += \
    src/controller/authenticationservice.cpp \
    src/database.cpp \
    src/employee.cpp \
    src/main.cpp \
    src/utils.cpp \
    src/controller/authorizationcontroller.cpp \
    src/controller/requestmapper.cpp

# PostgreSql
LIBS += -L$$COMMON_LIBS_DIR \
        -lpq \
        -lpqxx

OTHER_FILES += etc/* etc/docroot/* etc/templates/* etc/ssl/* logs/*

DISTFILES += ../CHANGELOG.txt ../LICENSE.txt ../README.txt \
    etc/ssl/README.txt

#---------------------------------------------------------------------------------------
# The following lines include the sources of the QtWebAppLib library
#---------------------------------------------------------------------------------------

include(QtWebApp/logging/logging.pri)
include(QtWebApp/httpserver/httpserver.pri)
include(QtWebApp/templateengine/templateengine.pri)
include(qjsonwebtoken/qjsonwebtoken.pri)


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    database/employees.sql
