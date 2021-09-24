QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    audio.cpp \
    control.cpp \
    dialog.cpp \
    helper.cpp \
    main.cpp \
    mainwindow.cpp \
    manageAudioDevice.cpp \
    video.cpp

HEADERS += \
    audio.h \
    control.h \
    dialog.h \
    ffmpegDll.h \
    helper.h \
    mainwindow.h \
    manageAudioDevice.h \
    mixHeader.h \
    sdl2Dll.h \
    video.h

FORMS += \
    dialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $$PWD/include/ffmpeg \
                $$PWD/include/SDL2/include
