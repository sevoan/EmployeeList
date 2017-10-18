#-------------------------------------------------
#
# Project created by QtCreator 2017-10-05T11:20:48
#
#-------------------------------------------------

QT += core gui sql svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EmployeeList
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

RESOURCES = resources.qrc

SOURCES += \
    repository.cpp \
    main.cpp \
    mainwindow.cpp \
    employeedialog.cpp \
    nationality.cpp \
    nationalitydialog.cpp \
    employee.cpp \
    employeetablemodel.cpp \
    weekloaddialog.cpp \
    weekload.cpp \
    formvalidator.cpp \
    notificator.cpp \
    employeefilter.cpp \
    sqlshortcut.cpp \
    sqlshortcutdialog.cpp

HEADERS += \
    repository.h \
    mainwindow.h \
    repository_p.h \
    nationality.h \
    employee.h \
    employeedialog_p.h \
    employeedialog.h \
    nationalitydialog.h \
    employeetablemodel.h \
    weekloaddialog.h \
    weekload.h \
    weekload_p.h \
    employee_p.h \
    formvalidator.h \
    formvalidator_p.h \
    formvalidatorrules.h \
    notificator.h \
    notificator_p.h \
    employeefilter.h \
    sqlshortcut.h \
    sqlshortcut_p.h \
    sqlshortcutdialog.h

FORMS += \
    mainwindow.ui \
    employeedialog.ui \
    nationalitydialog.ui \
    weekloaddialog.ui \
    notificator.ui \
    sqlshortcutdialog.ui
