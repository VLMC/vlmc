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
    src/gui/ClipPreviewWidget.cpp \
    src/gui/TracksRuler.cpp \
    src/gui/Preferences.cpp \
    src/gui/ListViewMediaItem.cpp \
    src/gui/MediaListWidget.cpp \
    src/Clip.cpp \
    src/gui/About.cpp \
    src/gui/Transcode.cpp \
    src/gui/Slider.cpp \
    src/MetaDataManager.cpp \
    src/Library.cpp \
    src/gui/GraphicsMovieItem.cpp \
    src/gui/AbstractGraphicsMediaItem.cpp \
    src/Media.cpp \
    src/gui/FileBrowser.cpp \
    src/gui/GraphicsCursorItem.cpp \
    src/TimelineBackend.cpp \
    src/Track.cpp \
    src/Workflow/ClipWorkflow.cpp \
    src/Workflow/TrackWorkflow.cpp \
    src/Workflow/MainWorkflow.cpp \
    src/gui/PreviewWidget.cpp \
    src/gui/RenderPreviewWidget.cpp
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
    src/gui/ClipPreviewWidget.h \
    src/gui/TracksRuler.h \
    src/gui/Preferences.h \
    src/gui/ListViewMediaItem.h \
    src/Clip.h \
    src/Image.h \
    src/gui/MediaListWidget.h \
    src/gui/About.h \
    src/gui/Transcode.h \
    src/gui/Slider.h \
    src/MetaDataManager.h \
    src/Singleton.hpp \
    src/Library.h \
    src/gui/AbstractGraphicsMediaItem.h \
    src/gui/GraphicsMovieItem.h \
    src/Media.h \
    src/gui/FileBrowser.h \
    src/gui/GraphicsCursorItem.h \
    src/TimelineBackend.h \
    src/Track.h \
    src/Workflow/ClipWorkflow.h \
    src/Workflow/TrackWorkflow.h \
    src/Workflow/MainWorkflow.h \
    src/gui/PreviewWidget.h \
    src/gui/RenderPreviewWidget.h \
    src/gui/GenericPreviewWidget.h
FORMS += src/gui/ui/MainWindow.ui \
    src/gui/ui/PreviewWidget.ui \
    src/gui/ui/Preferences.ui \
    src/gui/ui/Timeline.ui \
    src/gui/ui/LibraryWidget.ui \
    src/gui/ui/About.ui \
    src/gui/ui/Transcode.ui \
    src/gui/ui/FileBrowser.ui
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
