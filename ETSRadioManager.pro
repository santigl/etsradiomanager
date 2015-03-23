#-------------------------------------------------
#
# Project created by QtCreator 2015-02-20T16:04:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ETSRadioManager
TEMPLATE = app

VERSION 					= 0.0.0.1
QMAKE_TARGET_PRODUCT 		= "ETS Radio Manager"
QMAKE_TARGET_DESCRIPTION 	= "Conveniently manage live_streams.sii files."
QMAKE_TARGET_COPYRIGHT		= "Copyright (C) 2015 SantiX"
RC_ICONS 					= "Resources/ETSRadioManager.ico"


SOURCES += main.cpp\
        mainwindow.cpp \
    aboutdialog.cpp \
    parser.cpp \
    insertdialog.cpp

HEADERS  += mainwindow.h \
    parser.h \
    aboutdialog.h \
    insertdialog.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    insertdialog.ui

RESOURCES += \
    Icons.qrc
