#CMakeQt by Rohit Yadav<rohityadav89@gmail.com>
#CMake Build System for Qt

SUBDIRS(EffectsEngine/Plugins/src)

SET(VLMC_SRCS 
    main.cpp
    Actions/ActionStack.cpp
    Actions/StackedAction.hpp
    API/Module.cpp
    API/ModuleManager.cpp
    API/vlmc_module_variables.cpp
    Commands/Commands.cpp
    Commands/KeyboardShortcutHelper.cpp
    Configuration/ProjectSettingsDefault.cpp
    Configuration/SettingsManager.cpp
    Configuration/SettingValue.cpp
    Configuration/VLMCSettingsDefault.cpp
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
    Gui/LanguagePreferences.cpp
    Gui/LCDTimecode.cpp
    Gui/MainWindow.cpp
    Gui/PreviewRuler.cpp
    Gui/PreviewWidget.cpp
    Gui/TagWidget.cpp
    Gui/UndoStack.cpp
    Gui/WorkflowFileRendererDialog.cpp
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
    Gui/settings/AudioProjectPreferences.cpp
    Gui/settings/KeyboardShortcut.cpp
    Gui/settings/KeyboardShortcutInput.cpp
    Gui/settings/Panel.cpp
    Gui/settings/PreferenceWidget.cpp
    Gui/settings/ProjectPreferences.cpp
    Gui/settings/Settings.cpp
    Gui/settings/VideoProjectPreferences.cpp
    Gui/settings/VLMCPreferences.cpp
    Gui/timeline/AbstractGraphicsMediaItem.cpp
    Gui/timeline/GraphicsAudioItem.cpp
    Gui/timeline/GraphicsCursorItem.cpp
    Gui/timeline/GraphicsMovieItem.cpp
    Gui/timeline/GraphicsTrack.hpp
    Gui/timeline/Timeline.cpp
    Gui/timeline/TracksControls.cpp
    Gui/timeline/TracksRuler.cpp
    Gui/timeline/TracksScene.cpp
    Gui/timeline/TracksView.cpp
    Gui/widgets/CrashHandler.cpp
    Gui/widgets/TrackControls.cpp
    Gui/wizard/CustomWizardPage.cpp
    Gui/wizard/GeneralPage.cpp
    Gui/wizard/OpenPage.cpp
    Gui/wizard/ProjectWizard.cpp
    Gui/wizard/VideoPage.cpp
    Gui/wizard/WelcomePage.cpp
    Library/Library.cpp
    LibVLCpp/VLCException.cpp
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
    Workflow/TrackHandler.cpp
    Workflow/TrackWorkflow.cpp
    Workflow/VideoClipWorkflow.cpp
  )

SET (VLMC_HDRS
    Commands/KeyboardShortcutHelper.h                       
    Configuration/SettingsManager.h                         
    Configuration/SettingValue.h                            
    Gui/About.h                                                                   
    Gui/ClickableLabel.h                                                          
    Gui/ClipProperty.h                                                            
    Gui/DockWidgetManager.h                                                       
    Gui/FileInfoListModel.h                                                       
    Gui/import/ImportController.h                                                 
    Gui/import/ImportMediaCellView.h                                              
    Gui/import/ImportMediaListController.h                                        
    Gui/LanguagePreferences.h                                                     
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
    Gui/settings/KeyboardShortcut.h                                               
    Gui/settings/KeyboardShortcutInput.h                                          
    Gui/settings/Panel.h                                                          
    Gui/settings/PreferenceWidget.h                                               
    Gui/settings/Settings.h                                                       
    Gui/settings/VLMCPreferences.h                                                
    Gui/TagWidget.h                                                               
    Gui/timeline/AbstractGraphicsMediaItem.h                                      
    Gui/timeline/GraphicsAudioItem.h                                              
    Gui/timeline/GraphicsCursorItem.h                                             
    Gui/timeline/GraphicsMovieItem.h                                              
    Gui/timeline/GraphicsTrack.hpp
    Gui/timeline/Timeline.h                                                       
    Gui/timeline/TracksControls.h                                                 
    Gui/timeline/TracksRuler.h
    Gui/timeline/TracksScene.h
    Gui/timeline/TracksView.h
    Gui/UndoStack.h
    Gui/widgets/CrashHandler.h
    Gui/widgets/TrackControls.h
    Gui/wizard/CustomWizardPage.h
    Gui/wizard/GeneralPage.h
    Gui/wizard/OpenPage.h
    Gui/wizard/ProjectWizard.h
    Gui/wizard/VideoPage.h
    Gui/wizard/WelcomePage.h
    Gui/WorkflowFileRendererDialog.h
    Library/Library.h
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
    Workflow/StackedBuffer.hpp
    Workflow/TrackHandler.h
    Workflow/TrackWorkflow.h
    Workflow/VideoClipWorkflow.h
  )

SET(VLMC_UIS
    Gui/import/ui/ImportController.ui
    Gui/library/StackViewNavController.ui
    Gui/library/ui/MediaCellView.ui
    Gui/library/ui/StackViewNavController.ui
    Gui/settings/ui/AudioProjectPreferences.ui
    Gui/settings/ui/LanguagePreferences.ui
    Gui/settings/ui/ProjectPreferences.ui
    Gui/settings/ui/VideoProjectPreferences.ui
    Gui/settings/ui/VLMCPreferences.ui
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
    Gui/widgets/CrashHandler.ui
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

#include paths for vlmc
INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}/src
  .
  Actions
  API
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

ADD_EXECUTABLE(vlmc ${VLMC_SRCS} ${VLMC_MOC_SRCS} ${VLMC_UIS_H} ${VLMC_RCC_SRCS})

TARGET_LINK_LIBRARIES(vlmc
  ${QT_QTCORE_LIBRARY} 
  ${QT_QTGUI_LIBRARY} 
  ${QT_QTXML_LIBRARY} 
  ${QT_QTSVG_LIBRARY} 
  ${QT_QTNETWORK_LIBRARY}
  ${LIBVLC_LIBRARY}
  )

#INSTALL(FILES ${VLMC_SRC_HDRS} DESTINATION ${VLMC_INCLUDE_DIR}) 
#INSTALL(FILES vlmc DESTINATION ${VLMC_BIN_DIR})
INSTALL(FILES vlmc RUNTIME DESTINATION ${VLMC_BIN_DIR})

 ADD_CUSTOM_COMMAND(
   OUTPUT ${CMAKE_SOURCE_DIR}/bin/vlmc
   COMMAND ${CMAKE_COMMAND} copy ${CMAKE_CURRENT_SOURCE_DIR}/vlmc ${CMAKE_SOURCE_DIR}/bin/vlmc
   DEPENDS ${CMAKE_BINARY_DIR}/src/vlmc
   )


