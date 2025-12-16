#-------------------------------------------------
#
# Project created by QtCreator 2025-12-10T01:05:07
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Organization
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

# Настройка пути сборки
DESTDIR = $$PWD/build
OBJECTS_DIR = $$PWD/build/.obj
MOC_DIR = $$PWD/build/.moc
RCC_DIR = $$PWD/build/.rcc
UI_DIR = $$PWD/build/.ui

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        models/user.cpp \
        models/car.cpp \
        models/rental.cpp \
        models/fine.cpp \
        database/databasemanager.cpp \
        patterns/pricingstrategy.cpp \
        patterns/carstatusobserver.cpp \
        services/rentalservice.cpp \
        services/pricingcalculator.cpp \
        services/carservice.cpp \
        services/userservice.cpp \
        services/rentalsearchservice.cpp \
        managers/reportmanager.cpp \
        ui/loginwindow.cpp \
        ui/clientmainwindow.cpp \
        ui/adminmainwindow.cpp \
        ui/rentaldialog.cpp \
        ui/cardialog.cpp \
        ui/reportswindow.cpp \
        ui/importexportdialog.cpp \
        ui/registerdialog.cpp \
        utils/dataexporter.cpp \
        utils/dataimporter.cpp \
        utils/dateutils.cpp

HEADERS += \
        mainwindow.h \
        models/user.h \
        models/car.h \
        models/rental.h \
        models/fine.h \
        database/databasemanager.h \
        patterns/pricingstrategy.h \
        patterns/carstatusobserver.h \
        services/rentalservice.h \
        services/pricingcalculator.h \
        services/carservice.h \
        services/userservice.h \
        services/rentalsearchservice.h \
        managers/reportmanager.h \
        ui/loginwindow.h \
        ui/clientmainwindow.h \
        ui/adminmainwindow.h \
        ui/rentaldialog.h \
        ui/cardialog.h \
        ui/reportswindow.h \
        ui/importexportdialog.h \
        ui/registerdialog.h \
        utils/dataexporter.h \
        utils/dataimporter.h \
        utils/dateutils.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
