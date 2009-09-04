TARGET = vlmc
DESTDIR = bin
CONFIG += debug
TEMPLATE = app
VERSION = 0.0.1
OBJECTS_DIR = build
MOC_DIR = build/moc
UI_DIR = build/ui
QMAKE_EXT_H = .h \
    .hpp
INCLUDEPATH = build/moc \
    build/ui
QT += gui \
    network \
    svg \
    xml
SOURCES += src/main.cpp \
    src/GUI/MainWindow.cpp \
    src/GUI/LibraryWidget.cpp \
    src/GUI/DockWidgetManager.cpp \
    src/LibVLCpp/VLCException.cpp \
    src/LibVLCpp/VLCInstance.cpp \
    src/GUI/Timeline.cpp \
    src/LibVLCpp/VLCMediaPlayer.cpp \
    src/LibVLCpp/VLCMedia.cpp \
    src/GUI/TracksView.cpp \
    src/GUI/TracksScene.cpp \
    src/Renderer/ClipRenderer.cpp \
    src/GUI/TracksRuler.cpp \
    src/GUI/LanguagePreferences.cpp \
    src/GUI/ListViewMediaItem.cpp \
    src/GUI/MediaListWidget.cpp \
    src/Media/Clip.cpp \
    src/GUI/About.cpp \
#    src/GUI/Transcode.cpp \
    src/GUI/Slider.cpp \
    src/Metadata/MetaDataWorker.cpp \
    src/Library/Library.cpp \
    src/GUI/GraphicsMovieItem.cpp \
    src/GUI/AbstractGraphicsMediaItem.cpp \
    src/Media/Media.cpp \
    src/GUI/FileBrowser.cpp \
    src/GUI/GraphicsCursorItem.cpp \
    src/Workflow/ClipWorkflow.cpp \
    src/Workflow/TrackWorkflow.cpp \
    src/Workflow/MainWorkflow.cpp \
    src/GUI/PreviewWidget.cpp \
    src/Renderer/WorkflowRenderer.cpp \
    src/API/vlmc_module_variables.cpp \
    src/API/Module.cpp \
    src/API/ModuleManager.cpp \
    src/Renderer/WorkflowFileRenderer.cpp \
    src/GUI/UndoStack.cpp \
    src/Metadata/MetaDataManager.cpp \
    src/GUI/ClipProperty.cpp \
    src/GUI/WorkflowFileRendererDialog.cpp \
    src/GUI/Settings.cpp \
    src/GUI/Panel.cpp \
    src/GUI/PreferenceWidget.cpp \
    src/GUI/ProjectPreferences.cpp \
    src/Commands/Commands.cpp \
    src/GUI/AudioProjectPreferences.cpp \
    src/GUI/VLMCPreferences.cpp \
    src/GUI/VideoProjectPreferences.cpp \
    src/Project/ProjectManager.cpp
HEADERS += src/GUI/MainWindow.h \
    src/GUI/DockWidgetManager.h \
    src/GUI/LibraryWidget.h \
    src/LibVLCpp/VLCpp.hpp \
    src/LibVLCpp/VLCException.h \
    src/LibVLCpp/VLCInstance.h \
    src/GUI/Timeline.h \
    src/LibVLCpp/VLCMediaPlayer.h \
    src/LibVLCpp/VLCMedia.h \
    src/GUI/TracksView.h \
    src/GUI/TracksScene.h \
    src/Renderer/ClipRenderer.h \
    src/GUI/TracksRuler.h \
    src/GUI/LanguagePreferences.h \
    src/GUI/ListViewMediaItem.h \
    src/Media/Clip.h \
    src/GUI/MediaListWidget.h \
    src/GUI/About.h \
#    src/GUI/Transcode.h \
    src/GUI/Slider.h \
    src/Metadata/MetaDataWorker.h \
    src/Tools/Singleton.hpp \
    src/Library/Library.h \
    src/GUI/AbstractGraphicsMediaItem.h \
    src/GUI/GraphicsMovieItem.h \
    src/Media/Media.h \
    src/GUI/FileBrowser.h \
    src/GUI/GraphicsCursorItem.h \
    src/Workflow/ClipWorkflow.h \
    src/Workflow/TrackWorkflow.h \
    src/Workflow/MainWorkflow.h \
    src/GUI/PreviewWidget.h \
    src/Renderer/WorkflowRenderer.h \
    src/Renderer/GenericRenderer.h \
    src/Tools/Toggleable.hpp \
    src/API/vlmc_module.h \
    src/API/Module.h \
    src/API/ModuleManager.h \
    src/API/vlmc_module_internal.h \
    src/Renderer/WorkflowFileRenderer.h \
    src/vlmc.h \
    src/Tools/Pool.hpp \
    src/GUI/UndoStack.h \
    src/Tools/WaitCondition.hpp \
    src/Metadata/MetaDataManager.h \
    src/Tools/QSingleton.hpp \
    src/GUI/ClipProperty.h \
    src/GUI/WorkflowFileRendererDialog.h \
    src/GUI/Settings.h \
    src/GUI/Panel.h \
    src/GUI/GraphicsTrack.hpp \
    src/Commands/Commands.h \
    src/GUI/VLMCPreferences.h \
    src/GUI/AudioProjectPreferences.h \
    src/GUI/VideoProjectPreferences.h \
    src/Project/ProjectManager.h
FORMS += src/GUI/ui/MainWindow.ui \
    src/GUI/ui/PreviewWidget.ui \
    src/GUI/ui/LanguagePreferences.ui \
    src/GUI/ui/Timeline.ui \
    src/GUI/ui/LibraryWidget.ui \
    src/GUI/ui/About.ui \
#    src/GUI/ui/Transcode.ui \
    src/GUI/ui/FileBrowser.ui \
    src/GUI/ui/WorkflowFileRendererDialog.ui \
    src/GUI/ui/ClipProperty.ui \
    src/GUI/ui/ProjectPreferences.ui \
    src/GUI/ui/VLMCPreferences.ui \
    src/GUI/ui/VideoProjectPreferences.ui \
    src/GUI/ui/AudioProjectPreferences.ui
TRANSLATIONS = ts/vlmc_es.ts \
    ts/vlmc_fr.ts \
    ts/vlmc_sv.ts
RESOURCES += ressources.qrc
INCLUDEPATH += src/LibVLCpp \
    src/GUI \
    src/Tools \
    src/Renderer \
    src/Metadata \
    src/Commands \
    src/Workflow \
    src/Library \
    src/Media \
    src/Project \
    src

# QMAKE_CFLAGS+=-pg
# QMAKE_CXXFLAGS+=-pg
# QMAKE_LFLAGS+=-pg
LIBS = -L/usr/local/lib \
    -lvlc
SUBDIRS += modules
DEFINES += VLMC_VERSION="$$VERSION"
CODECFORTR = UTF-8
include(locale.pri)
