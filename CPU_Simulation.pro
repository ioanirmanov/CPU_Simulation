#-------------------------------------------------
#
# Project created by QtCreator 2018-12-09T17:41:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CPU_Simulation
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
    controlunit.cpp \
    register.cpp \
    memorybus.cpp \
    databus.cpp \
    addressbus.cpp \
    controlbus.cpp \
    arithlogunit.cpp \
    indatabus.cpp \
    mainwindow.cpp \
    alucontrolbus.cpp \
    memory.cpp \
    binaryutilities.cpp \
    memoryhelper.cpp \
    diagram.cpp \
    controlwidget.cpp \
    memorywidget.cpp \
    registerinfowidget.cpp \
    instructionhelper.cpp \
    memoryinfowindow.cpp \
    assembler.cpp \
    assemblerwindow.cpp

HEADERS += \
    controlunit.h \
    register.h \
    memorybus.h \
    databus.h \
    addressbus.h \
    controlbus.h \
    arithlogunit.h \
    indatabus.h \
    mainwindow.h \
    alucontrolbus.h \
    memory.h \
    binaryutilities.h \
    memoryhelper.h \
    diagram.h \
    controlwidget.h \
    memorywidget.h \
    registerinfowidget.h \
    instructionhelper.h \
    memoryinfowindow.h \
    assembler.h \
    assemblerwindow.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
