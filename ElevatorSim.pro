QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ECS_Centralized.cpp \
    Elevator_Centralized.cpp \
    Floor_Centralized.cpp \
    main.cpp \
    mainwindow.cpp \
    myThread.cpp

HEADERS += \
    ../CentralizedHeaders/ECS_Centralized.h \
    ../CentralizedHeaders/Elevator_Centralized.h \
    ../CentralizedHeaders/Floor_Centralized.h \
    ../CentralizedHeaders/defs_Centralized.h \
    ECS_Centralized.h \
    Elevator_Centralized.h \
    Floor_Centralized.h \
    defs_Centralized.h \
    mainwindow.h \
    myThread.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
