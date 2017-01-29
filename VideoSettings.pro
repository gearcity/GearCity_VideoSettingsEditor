#-------------------------------------------------
#
# Project created by QtCreator 2016-12-21T23:32:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += xml

TARGET = VideoSettings
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
        PredefinedResolutions.cpp \
        LanguageFiller.cpp \
    SettingsImporter.cpp \
    ResetToDefault.cpp \
    SaveSettings.cpp

HEADERS  += MainWindow.h \
         PredefinedResolutions.h \
         LanguageFiller.h \
    SettingsImporter.h \
    ResetToDefault.h \
    SavesSettings.h



FORMS    += MainWindow.ui


