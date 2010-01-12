project(MixerEffectPlugin)

set(MIXEREFFECT_SRCS
    MixerEffectPlugin.cpp
    MixerEffectPluginCreator.cpp
    ../../../PluginsAPI/LightVideoFrame.cpp
)

qt4_automoc(${MIXEREFFECT_SRCS})

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

ADD_LIBRARY(MixerEffectPlugin SHARED ${MIXEREFFECT_SRCS})

target_link_libraries(MixerEffectPlugin ${QT_LIBRARIES})

INCLUDE_DIRECTORIES(
    ${QT_INCLUDE_DIR}
    ../../../PluginsAPI
)
