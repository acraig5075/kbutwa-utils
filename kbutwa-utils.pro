#-------------------------------------------------
#
# Project created by QtCreator 2015-11-30T18:03:56
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kbutwa-utils
TEMPLATE = app

QTPLUGIN += QSQLMYSQL

SOURCES += main.cpp\
        widget.cpp \
    databasedlg.cpp \
    utils.cpp \
    movetargetdlg.cpp \
    viewtestdlg.cpp \
    searchresultsdlg.cpp

HEADERS  += widget.h \
    databasedlg.h \
    utils.h \
    movetargetdlg.h \
    viewtestdlg.h \
    searchresultsdlg.h

FORMS    += widget.ui \
    databasedlg.ui \
    movetargetdlg.ui \
    viewtestdlg.ui \
    searchresultsdlg.ui

RESOURCES += \
    assets/icons.qrc
