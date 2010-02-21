#CMakeQt by Rohit Yadav<rohityadav89@gmail.com>
#CMake Build System for Qt

SUBDIRS(EffectsEngine/Plugins/src)

SET(VLMC_SRCS
    main.cpp
    Commands/Commands.cpp
    Commands/KeyboardShortcutHelper.cpp
    Configuration/SettingsManager.cpp
    Configuration/SettingValue.cpp
    EffectsEngine/EffectNode.cpp
    EffectsEngine/EffectNodeFactory.cpp
    EffectsEngine/EffectPluginTypeLoader.cpp
    EffectsEngine/EffectPluginTypeManager.cpp
    EffectsEngine/EffectsEngine.cpp
    EffectsEngine/SemanticObjectManager.hpp
    EffectsEngine/SimpleObjectsReferencer.hpp
    EffectsEngine/PluginsAPI/InSlot.hpp
    EffectsEngine/PluginsAPI/LightVideoFrame.cpp
    EffectsEngine/PluginsAPI/OutSlot.hpp
    Gui/About.cpp
    Gui/AudioSpectrumDrawer.cpp
    Gui/ClickableLabel.cpp
    Gui/ClipProperty.cpp
    Gui/DockWidgetManager.cpp
    Gui/FileInfoListModel.cpp
    Gui/LanguageHelper.cpp
    Gui/LCDTimecode.cpp
    Gui/MainWindow.cpp
    Gui/PreviewRuler.cpp
    Gui/PreviewWidget.cpp
    Gui/TagWidget.cpp
    Gui/UndoStack.cpp
    Gui/WorkflowFileRendererDialog.cpp
    Gui/export/RendererSettings.cpp
    Gui/import/ImportController.cpp
    Gui/import/ImportMediaCellView.cpp
    Gui/import/ImportMediaListController.cpp
    Gui/library/ClipListViewController.cpp
    Gui/library/ListViewController.cpp
    Gui/library/MediaCellView.cpp
    Gui/library/MediaLibraryWidget.cpp
    Gui/library/MediaListViewController.cpp
    Gui/library/StackViewController.cpp
    Gui/library/StackViewNavController.cpp
    Gui/settings/BoolWidget.cpp
    Gui/settings/DoubleWidget.cpp
    Gui/settings/IntWidget.cpp
    Gui/settings/KeyboardShortcut.cpp
    Gui/settings/KeyboardShortcutInput.cpp
    Gui/settings/LanguageWidget.cpp
    Gui/settings/Panel.cpp
    Gui/settings/PreferenceWidget.cpp
    Gui/settings/Settings.cpp
    Gui/settings/StringWidget.cpp
    Gui/timeline/AbstractGraphicsMediaItem.cpp
    Gui/timeline/GraphicsAudioItem.cpp
    Gui/timeline/GraphicsCursorItem.cpp
    Gui/timeline/GraphicsMovieItem.cpp
    Gui/timeline/GraphicsTrack.cpp
    Gui/timeline/Timeline.cpp
    Gui/timeline/TracksControls.cpp
    Gui/timeline/TracksRuler.cpp
    Gui/timeline/TracksScene.cpp
    Gui/timeline/TracksView.cpp
    Gui/widgets/TrackControls.cpp
    Gui/wizard/GeneralPage.cpp
    Gui/wizard/OpenPage.cpp
    Gui/wizard/ProjectWizard.cpp
    Gui/wizard/VideoPage.cpp
    Gui/wizard/WelcomePage.cpp
    Library/Library.cpp
    LibVLCpp/VLCInstance.cpp
    LibVLCpp/VLCMedia.cpp
    LibVLCpp/VLCMediaPlayer.cpp
    LibVLCpp/VLCpp.hpp
    Media/Clip.cpp
    Media/Media.cpp
    Metadata/MetaDataManager.cpp
    Metadata/MetaDataWorker.cpp
    Project/ProjectManager.cpp
    Renderer/ClipRenderer.cpp
    Renderer/GenericRenderer.cpp
    Renderer/WorkflowFileRenderer.cpp
    Renderer/WorkflowRenderer.cpp
    Tools/Pool.hpp
    Tools/QSingleton.hpp
    Tools/Singleton.hpp
    Tools/Toggleable.hpp
    Tools/VlmcDebug.cpp
    Tools/WaitCondition.hpp
    Workflow/AudioClipWorkflow.cpp 
    Workflow/ClipWorkflow.cpp
    Workflow/ImageClipWorkflow.cpp
    Workflow/MainWorkflow.cpp
    Workflow/StackedBuffer.hpp
    Workflow/TrackHandler.cpp
    Workflow/TrackWorkflow.cpp
    Workflow/VideoClipWorkflow.cpp
  )

IF (WIN32)
    LIST ( APPEND VLMC_SRCS winvlmc.cpp )
ELSE (WIN32)
    LIST( APPEND VLMC_SRCS vlmc.cpp )
ENDIF(WIN32)

SET (VLMC_HDRS
    Commands/KeyboardShortcutHelper.h
    Configuration/SettingsManager.h
    Configuration/SettingValue.h
    Gui/About.h
    Gui/ClickableLabel.h
    Gui/ClipProperty.h
    Gui/DockWidgetManager.h
    Gui/export/RendererSettings.h
    Gui/FileInfoListModel.h
    Gui/import/ImportController.h
    Gui/import/ImportMediaCellView.h
    Gui/import/ImportMediaListController.h
    Gui/LanguageHelper.h
    Gui/LCDTimecode.h
    Gui/library/ClipListViewController.h
    Gui/library/ListViewController.h
    Gui/library/MediaCellView.h
    Gui/library/MediaLibraryWidget.h
    Gui/library/MediaListViewController.h
    Gui/library/StackViewController.h
    Gui/library/StackViewNavController.h
    Gui/library/ViewController.h
    Gui/MainWindow.h
    Gui/PreviewRuler.h
    Gui/PreviewWidget.h
    Gui/settings/BoolWidget.h
    Gui/settings/DoubleWidget.h
    Gui/settings/IntWidget.h
    Gui/settings/ISettingsCategorieWidget.h
    Gui/settings/KeyboardShortcut.h
    Gui/settings/KeyboardShortcutInput.h
    Gui/settings/LanguageWidget.h
    Gui/settings/Panel.h
    Gui/settings/PreferenceWidget.h
    Gui/settings/Settings.h
    Gui/settings/StringWidget.h
    Gui/TagWidget.h
    Gui/timeline/AbstractGraphicsMediaItem.h
    Gui/timeline/GraphicsAudioItem.h 
    Gui/timeline/GraphicsCursorItem.h
    Gui/timeline/GraphicsMovieItem.h 
    Gui/timeline/GraphicsTrack.h
    Gui/timeline/Timeline.h
    Gui/timeline/TracksControls.h
    Gui/timeline/TracksRuler.h
    Gui/timeline/TracksScene.h
    Gui/timeline/TracksView.h
    Gui/UndoStack.h
    Gui/widgets/TrackControls.h
    Gui/wizard/GeneralPage.h
    Gui/wizard/OpenPage.h
    Gui/wizard/ProjectWizard.h
    Gui/wizard/VideoPage.h
    Gui/wizard/WelcomePage.h
    Gui/WorkflowFileRendererDialog.h
    Library/Library.h
    LibVLCpp/VLCInstance.h
    LibVLCpp/VLCMediaPlayer.h
    Media/Clip.h
    Media/Media.h
    Metadata/MetaDataManager.h
    Metadata/MetaDataWorker.h
    Project/ProjectManager.h
    Renderer/ClipRenderer.h
    Renderer/GenericRenderer.h
    Renderer/WorkflowFileRenderer.h
    Renderer/WorkflowRenderer.h
    Tools/VlmcDebug.h
    Workflow/AudioClipWorkflow.h 
    Workflow/ClipWorkflow.h
    Workflow/ImageClipWorkflow.h
    Workflow/MainWorkflow.h
    Workflow/TrackHandler.h
    Workflow/TrackWorkflow.h
    Workflow/VideoClipWorkflow.h
  )

SET(VLMC_UIS
    Gui/export/RendererSettings.ui
    Gui/import/ui/ImportController.ui
    Gui/library/StackViewNavController.ui
    Gui/library/ui/MediaCellView.ui
    Gui/library/ui/StackViewNavController.ui
    Gui/ui/About.ui
    Gui/ui/ClipProperty.ui
    Gui/ui/FileBrowser.ui
    Gui/ui/LibraryWidget.ui
    Gui/ui/MainWindow.ui
    Gui/ui/PreviewWidget.ui
    Gui/ui/TagWidget.ui
    Gui/ui/Timeline.ui
    Gui/ui/transcode.ui
    Gui/ui/WorkflowFileRendererDialog.ui
    Gui/widgets/TrackControls.ui
    Gui/wizard/ui/GeneralPage.ui
    Gui/wizard/ui/OpenPage.ui
    Gui/wizard/ui/VideoPage.ui
    Gui/wizard/ui/WelcomePage.ui
  )

SET(VLMC_RCC
    ../ressources.qrc
    ../ts/ressources-ts.qrc
  )

IF (WIN32)
    LIST( APPEND VLMC_RCC ../resources/styles.qrc )
ENDIF (WIN32)

IF (WITH_CRASHHANDLER_GUI)
    LIST( APPEND VLMC_SRCS Gui/widgets/CrashHandler.cpp )
    IF ( CMAKE_SYSTEM_NAME STREQUAL "Linux" )
        LIST( APPEND VLMC_SRCS Tools/UnixBacktraceGenerator.cpp )
    ELSE ( CMAKE_SYSTEM_NAME STREQUAL "Linux" )
        LIST( APPEND VLMC_SRCS Tools/Win32BacktraceGenerator.cpp )
    ENDIF( CMAKE_SYSTEM_NAME STREQUAL "Linux" )
    LIST( APPEND VLMC_HDRS Gui/widgets/CrashHandler.h Tools/BacktraceGenerator.h )
    LIST( APPEND VLMC_UIS Gui/widgets/CrashHandler.ui )
    
ENDIF(WITH_CRASHHANDLER_GUI)

#include paths for vlmc
INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/src
  .
  Commands
  Configuration
  EffectsEngine
  EffectsEngine/PluginsAPI
  Gui
  Gui/import
  Gui/library
  Gui/settings
  Gui/timeline
  Gui/widgets
  Gui/wizard
  Library
  LibVLCpp
  Media
  Metadata
  Project
  Renderer
  Tools
  Workflow
)

QT4_WRAP_UI(VLMC_UIS_H ${VLMC_UIS})
QT4_ADD_RESOURCES(VLMC_RCC_SRCS ${VLMC_RCC})
QT4_WRAP_CPP(VLMC_MOC_SRCS ${VLMC_HDRS})

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/cmake/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

IF(WIN32)
    SET( GUI_TYPE WIN32 )
ENDIF( WIN32 )
ADD_EXECUTABLE(vlmc ${GUI_TYPE} ${VLMC_SRCS} ${VLMC_MOC_SRCS} ${VLMC_UIS_H} ${VLMC_RCC_SRCS})
ADD_DEPENDENCIES(vlmc translations)

TARGET_LINK_LIBRARIES(vlmc
  ${QT_QTCORE_LIBRARY} 
  ${QT_QTGUI_LIBRARY} 
  ${QT_QTXML_LIBRARY} 
  ${QT_QTSVG_LIBRARY} 
  ${QT_QTNETWORK_LIBRARY}
  ${LIBVLC_LIBRARY}
  ${LIBVLCCORE_LIBRARY}
  )

INSTALL(TARGETS vlmc RUNTIME DESTINATION ${VLMC_BIN_DIR})

ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_SOURCE_DIR}/bin/vlmc
    COMMAND ${CMAKE_COMMAND} copy ${CMAKE_CURRENT_SOURCE_DIR}/vlmc ${CMAKE_SOURCE_DIR}/bin/vlmc
    DEPENDS ${CMAKE_BINARY_DIR}/src/vlmc
)


