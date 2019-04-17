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

DEFINES += STEAMSUPPORT


SOURCES += main.cpp\
        MainWindow.cpp \
        PredefinedResolutions.cpp \
        LanguageFiller.cpp \
    SettingsImporter.cpp \
    ResetToDefault.cpp \
    SaveSettings.cpp \
    ReadModFile.cpp \
    ClearCache.cpp \
    SteamWorkshopLoader.cpp


HEADERS  += MainWindow.h \
         PredefinedResolutions.h \
         LanguageFiller.h \
    SettingsImporter.h \
    ResetToDefault.h \
    SavesSettings.h \
    OSXHelper.h \
    ReadModFile.h \
    ClearCache.h \
    SteamWorkshopLoader.h



FORMS    += MainWindow.ui


macx{
    SOURCES +=  OSXHelper.cpp
    QMAKE_LFLAGS += -F/System/Library/Frameworks/
    LIBS += -framework CoreFoundation
    LIBS += $$PWD/Steamworks_142/sdk/redistributable_bin/osx32/libsteam_api142.dylib
    INCLUDEPATH += $$PWD/Steamworks_142/sdk/public/

} else {

    unix{ #doesn't like Linux for some reason...

        LIBS += $$PWD/Steamworks_142/sdk/redistributable_bin/linux32/libsteam_api.so
        INCLUDEPATH += $$PWD/Steamworks_142/sdk/public/
    }

    win32{
        LIBS += $$PWD/Steamworks_142/sdk/redistributable_bin/steam_api.lib
        INCLUDEPATH += $$PWD/Steamworks_142/sdk/public/
    }


}




