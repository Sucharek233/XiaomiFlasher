QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    flashlog.cpp \
    fthread.cpp \
    main.cpp \
    mainwindow.cpp \
    resume.cpp

HEADERS += \
    flashlog.h \
    fthread.h \
    mainwindow.h \
    resume.h

FORMS += \
    flashlog.ui \
    mainwindow.ui \
    resume.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    programs.qrc
