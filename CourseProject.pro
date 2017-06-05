#-------------------------------------------------
#
# Project created by QtCreator 2017-05-04T14:05:05
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += widgets
QT       += core
QT       += printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CourseProject
TEMPLATE = app

INCLUDEPATH += /home/timurmirlas/Programs/NCReport2.20.3.x64.Qt5.8.0.eval/include
unix:CONFIG(release, debug|release) : LIBS += -L /home/timurmirlas/Programs/NCReport2.20.3.x64.Qt5.8.0.eval/lib -lNCReport
else:unix:CONFIG(debug, debug|release): LIBS += -L /home/timurmirlas/Programs/NCReport2.20.3.x64.Qt5.8.0.eval/lib -lNCReportDebug
unix:{
  QMAKE_LFLAGS += -Wl,--rpath=/home/timurmirlas/Programs/NCReport2.20.3.x64.Qt5.8.0.eval/lib
}
SOURCES += main.cpp\
        mainwindow.cpp \
    loginwindow.cpp \
    newrun_routewindow.cpp \
    newrun_buswindow.cpp \
    newrun_driverswindow.cpp \
    newroute.cpp \
    newroute_stopoverwindow.cpp \
    unscheduledstop.cpp \
    startrun.cpp \
    enhancetablewidget.cpp

HEADERS  += mainwindow.h \
    loginwindow.h \
    newrun_routewindow.h \
    newrun_buswindow.h \
    newrun_driverswindow.h \
    newroute.h \
    newroute_stopoverwindow.h \
    unscheduledstop.h \
    startrun.h \
    enhancetablewidget.h

FORMS    += mainwindow.ui \
    loginwindow.ui \
    newrun_routewindow.ui \
    newrun_buswindow.ui \
    newrun_driverswindow.ui \
    newroute.ui \
    newroute_stopoverwindow.ui \
    unscheduledstop.ui \
    startrun.ui
