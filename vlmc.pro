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
INCLUDEPATH += build/moc \
    build/ui \
    $$[VLMC_ADDITIONAL_INCLUDEPATH]
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
    src/GUI/PreviewRuler.cpp \
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
    src/Project/ProjectManager.cpp \
    src/Configuration/SettingsManager.cpp \
    src/GUI/TagWidget.cpp \
    src/GUI/Library/StackViewController.cpp \
    src/GUI/Library/StackViewNavController.cpp \
    src/GUI/Library/ListViewController.cpp \
    src/GUI/Library/MediaCellView.cpp \
    src/GUI/Library/MediaLibraryWidget.cpp \
    src/GUI/Library/MediaListViewController.cpp \
    src/GUI/ImportMediaListController.cpp \
    src/GUI/ImportMediaCellView.cpp \
    src/GUI/ClickableLabel.cpp \
    src/GUI/Import/ImportModel.cpp \
    src/GUI/Import/ImportController.cpp \
    src/Workflow/VideoClipWorkflow.cpp \
    src/Workflow/TrackHandler.cpp \
    src/Workflow/AudioClipWorkflow.cpp \
    src/EffectsEngine/EffectsEngine.cpp \
    src/EffectsEngine/LightVideoFrame.cpp \
    src/EffectsEngine/GenericEffect.cpp \
    src/EffectsEngine/GreenFilterEffect.cpp \
    src/EffectsEngine/MixerEffect.cpp \
    src/GUI/LCDTimecode.cpp
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
    src/GUI/PreviewRuler.h \
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
    src/Project/ProjectManager.h \
    src/Configuration/SettingsManager.h \
    src/GUI/TagWidget.h \
    src/GUI/Library/StackViewController.h \
    src/GUI/Library/StackViewNavController.h \
    src/GUI/Library/ListViewController.h \
    src/GUI/Library/MediaCellView.h \
    src/GUI/Library/MediaLibraryWidget.h \
    src/GUI/Library/MediaListViewController.h \
    src/GUI/Library/ViewController.h \
    src/GUI/ImportMediaListController.h \
    src/GUI/ImportMediaCellView.h \
    src/GUI/ClickableLabel.h \
    src/GUI/Import/ImportModel.h \
    src/GUI/Import/ImportController.h \
    src/Workflow/VideoClipWorkflow.h \
    src/Workflow/TrackHandler.h \
    src/Workflow/AudioClipWorkflow.h \
    src/EffectsEngine/EffectsEngine.h \
    src/EffectsEngine/InSlot.hpp \
    src/EffectsEngine/OutSlot.hpp \
    src/EffectsEngine/LightVideoFrame.h \
    src/EffectsEngine/GenericEffect.h \
    src/EffectsEngine/GreenFilterEffect.h \
    src/EffectsEngine/MixerEffect.h \
    src/GUI/LCDTimecode.h
FORMS += src/GUI/ui/MainWindow.ui \
    src/GUI/ui/PreviewWidget.ui \
    src/GUI/ui/LanguagePreferences.ui \
    src/GUI/ui/Timeline.ui \
    src/GUI/ui/LibraryWidget.ui \
    src/GUI/ui/About.ui \
    src/GUI/ui/FileBrowser.ui \
    src/GUI/ui/WorkflowFileRendererDialog.ui \
    src/GUI/ui/ClipProperty.ui \
    src/GUI/ui/ProjectPreferences.ui \
    src/GUI/ui/VLMCPreferences.ui \
    src/GUI/ui/VideoProjectPreferences.ui \
    src/GUI/ui/AudioProjectPreferences.ui \
    src/GUI/ui/TagWidget.ui \
    src/GUI/Library/ui/StackViewNavController.ui \
    src/GUI/Library/ui/MediaCellView.ui \
    src/GUI/Import/ui/ImportController.ui
TRANSLATIONS = ts/vlmc_es.ts \
    ts/vlmc_fr.ts \
    ts/vlmc_sv.ts
RESOURCES += ressources.qrc
INCLUDEPATH += src/LibVLCpp \
    src/GUI \
    src/GUI/Library \
    src/GUI/Import \
    src/Tools \
    src/Renderer \
    src/Metadata \
    src/Commands \
    src/Workflow \
    src/Library \
    src/Media \
    src/Project \
    src/EffectsEngine \
    src/Configuration \
    src/EffectsEngine \
    src

# QMAKE_CFLAGS+=-pg
# QMAKE_CXXFLAGS+=-pg
# QMAKE_LFLAGS+=-pg
# QMAKE_CXXFLAGS += -W -Wall -Wold-style-cast
LIBS += -L/usr/local/lib \
    -lvlc \
    $$[VLMC_ADDITIONAL_LIBS]
SUBDIRS += modules
DEFINES += VLMC_VERSION="$$VERSION"
CODECFORTR = UTF-8
include(locale.pri)
