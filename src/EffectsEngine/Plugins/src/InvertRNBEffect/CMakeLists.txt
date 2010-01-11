project(InvertRNBEffectPlugin)

if(NOT WIN32)

set(INVERTRNBEFFECT_SRCS InvertRNBEffectPlugin.cpp InvertRNBEffectPluginCreator.cpp) 

qt4_automoc($INVERTRNBEFFECT_SRCS})

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

ADD_LIBRARY(InvertRNBEffectPlugin SHARED ${INVERTRNBEFFECT_SRCS})

target_link_libraries(InvertRNBEffectPlugin ${QT_LIBRARIES})

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR} ../../../PluginsAPI)
#install(TARGETS mixereffect DESTINATION lib/kde4/plugins/styles)

endif(NOT WIN32)
