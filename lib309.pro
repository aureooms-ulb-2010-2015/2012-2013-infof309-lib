#-------------------------------------------------
#
# Project created by QtCreator 2013-03-15T21:35:39
#
#-------------------------------------------------

QT       -= gui

TARGET = lib309
TEMPLATE = lib
CONFIG += staticlib

SOURCES +=

HEADERS +=
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
