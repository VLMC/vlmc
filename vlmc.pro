TARGET = vlmc
DESTDIR = bin
CONFIG += debug
TEMPLATE = app
OBJECTS_DIR = build
MOC_DIR = build/moc
UI_DIR = build/ui
INCLUDEPATH = build/moc \
    build/ui
QT += gui \
    network \
    svg
SOURCES += src/main.cpp \
    src/gui/MainWindow.cpp \
    src/gui/LibraryWidget.cpp \
    src/gui/DockWidgetManager.cpp \
    src/LibVLCpp/VLCException.cpp \
    src/LibVLCpp/VLCInstance.cpp \
    src/gui/Timeline.cpp \
    src/LibVLCpp/VLCMediaPlayer.cpp \
    src/LibVLCpp/VLCMedia.cpp \
    src/gui/TracksView.cpp \
    src/gui/TracksScene.cpp \
    src/gui/PreviewWidget.cpp \
    src/gui/TracksRuler.cpp \
    src/gui/Preferences.cpp \
    src/gui/ListViewMediaItem.cpp \
    src/gui/MediaListWidget.cpp \
    src/Media.cpp \
    src/OutputMedia.cpp \
    src/gui/About.cpp \
    src/gui/Transcode.cpp \
    src/InputMedia.cpp \
    src/gui/Slider.cpp \
    src/MetaDataManager.cpp
HEADERS += src/gui/MainWindow.h \
    src/gui/DockWidgetManager.h \
    src/gui/LibraryWidget.h \
    src/LibVLCpp/VLCpp.hpp \
    src/LibVLCpp/VLCException.h \
    src/LibVLCpp/VLCInstance.h \
    src/gui/Timeline.h \
    src/LibVLCpp/VLCMediaPlayer.h \
    src/LibVLCpp/VLCMedia.h \
    src/gui/TracksView.h \
    src/gui/TracksScene.h \
    src/gui/PreviewWidget.h \
    src/gui/TracksRuler.h \
    src/gui/Preferences.h \
    src/gui/ListViewMediaItem.h \
    src/Media.h \
    src/Image.h \
    src/gui/MediaListWidget.h \
    src/OutputMedia.h \
    src/gui/About.h \
    src/gui/Transcode.h \
    src/InputMedia.h \
    src/gui/Slider.h \
    src/MetaDataManager.h
FORMS += src/gui/ui/MainWindow.ui \
    src/gui/ui/PreviewWidget.ui \
    src/gui/ui/Preferences.ui \
    src/gui/ui/Timeline.ui \
    src/gui/ui/LibraryWidget.ui \
    src/gui/ui/About.ui \
    src/gui/ui/Transcode.ui
FORMS +=
TRANSLATIONS = ts/vlmc_es.ts \
    ts/vlmc_fr.ts \
    ts/vlmc_sv.ts
RESOURCES += ressources.qrc
INCLUDEPATH += src/LibVLCpp \
    src/gui \
    src
LIBS = -L/usr/local/lib \
    -lvlc
CODECFORTR = UTF-8
include(locale.pri)
