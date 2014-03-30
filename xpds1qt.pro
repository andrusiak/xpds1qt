#-------------------------------------------------
#
# Project created by QtCreator 2013-11-21T22:06:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = xpds1qt
TEMPLATE = app


SOURCES  += mainwindow.cpp \
        main.cpp\
        qcustomplot.cpp \
        prests.c \
        pflmvs.c \
        pds1.c \
        padjus.c \
        mcc.c \
        initwin.c \
        fft.c

HEADERS  += mainwindow.h \
         qcustomplot.h \
         pds1.h

FORMS    += mainwindow.ui

#XGPATH = /usr/local/lib/xgrafix

CONFIG += console
