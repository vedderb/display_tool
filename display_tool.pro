#-------------------------------------------------
#
# Project created by QtCreator 2019-04-28T09:36:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG   += c++11

TARGET = display_tool
TEMPLATE = app

release_win {
    DESTDIR = build/win
    OBJECTS_DIR = build/win/obj
    MOC_DIR = build/win/obj
    RCC_DIR = build/win/obj
    UI_DIR = build/win/obj
}

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    displayedit.cpp \
    imagewidget.cpp \
    dispeditor.cpp

HEADERS += \
        mainwindow.h \
    displayedit.h \
    imagewidget.h \
    dispeditor.h

FORMS += \
        mainwindow.ui \
    dispeditor.ui
