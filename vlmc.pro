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
    network
SOURCES += src/main.cpp \
    src/gui/MainWindow.cpp \
    src/gui/LibraryWidget.cpp \
    src/gui/DockWidgetManager.cpp \
    src/LibVLCpp/VLCException.cpp \
    src/LibVLCpp/VlmManager.cpp \
    src/LibVLCpp/VLCInstance.cpp \
    src/LibVLCpp/VlmMedia.cpp \
    src/gui/Timeline.cpp \
    src/LibVLCpp/VLCMediaPlayer.cpp \
    src/LibVLCpp/VLCMedia.cpp \
    src/gui/TracksView.cpp \
    src/gui/TracksScene.cpp \
    src/gui/PreviewWidget.cpp \
    src/gui/TracksRuler.cpp \
    src/gui/Preferences.cpp \
    src/gui/ListViewMediaItem.cpp \
    src/gui/MediaListWidget.cpp
HEADERS += src/gui/MainWindow.h \
    src/gui/DockWidgetManager.h \
    src/gui/LibraryWidget.h \
    src/LibVLCpp/VLCpp.hpp \
    src/LibVLCpp/VLCException.h \
    src/LibVLCpp/VlmManager.h \
    src/LibVLCpp/VLCInstance.h \
    src/LibVLCpp/VlmMedia.h \
    src/gui/Timeline.h \
    src/LibVLCpp/VLCMediaPlayer.h \
    src/LibVLCpp/VLCMedia.h \
    src/gui/TracksView.h \
    src/gui/TracksScene.h \
    src/gui/PreviewWidget.h \
    src/gui/TracksRuler.h \
    src/gui/Preferences.h \
    src/gui/ListViewMediaItem.h \
    src/gui/MediaListWidget.h
FORMS += src/gui/ui/MainWindow.ui \
    src/gui/ui/Timeline.ui \
    src/gui/ui/LibraryWidget.ui \
    src/gui/ui/PreviewWidget.ui \
    src/gui/ui/Preferences.ui
RESOURCES += 
INCLUDEPATH += src/LibVLCpp
LIBS = -L/usr/local/lib \
    -lvlc
TRANSLATIONS = vlmc_es.ts \
    vlmc_fr.ts
CODECFORTR = UTF-8
