project(GreenFilterEffectPlugin)

set(GREENFILTEREFFECT_SRCS
    GreenFilterEffectPlugin.cpp
    GreenFilterEffectPluginCreator.cpp
    ../../../PluginsAPI/LightVideoFrame.cpp
)

qt4_automoc(${GREENFILTEREFFECT_SRCS})

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

ADD_LIBRARY(GreenFilterEffectPlugin SHARED ${GREENFILTEREFFECT_SRCS})

target_link_libraries(GreenFilterEffectPlugin ${QT_LIBRARIES})

INCLUDE_DIRECTORIES(
    ${QT_INCLUDE_DIR}
    ../../../PluginsAPI
)
