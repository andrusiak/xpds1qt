QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = xpds1qt
TEMPLATE = app


SOURCES  += mainwindow.cpp \
         chartwidget.cpp \
         main.cpp\
         qcustomplot.cpp \
         prests.c \
         pflmvs.c \
         pds1.c \
         padjus.c \
         mcc.c \
         initwin.c \
         fft.c \
         workthread.cpp \
    plasmamodel.cpp

HEADERS  += mainwindow.h \
         chartwidget.h \
         qcustomplot.h \
         pds1.h \
         workthread.h \
         pds.h \
    plasmamodel.h

FORMS    += mainwindow.ui \
         chartwidget.ui

OTHER_FILES+= inp/*

CONFIG += console
