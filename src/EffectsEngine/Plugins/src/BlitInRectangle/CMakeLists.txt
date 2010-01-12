project(BlitInRectangleEffectPlugin)

set(BLITINRECTANGLE_SRCS
    BlitInRectangleEffectPlugin.cpp
    BlitInRectangleEffectPluginCreator.cpp
    ../../../PluginsAPI/LightVideoFrame.cpp
)

qt4_automoc(${BLITINRECTANGLE_SRCS})

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

ADD_LIBRARY(BlitInRectangleEffectPlugin SHARED ${BLITINRECTANGLE_SRCS})

target_link_libraries(BlitInRectangleEffectPlugin ${QT_LIBRARIES})

INCLUDE_DIRECTORIES(
    ${QT_INCLUDE_DIR}
    ../../../PluginsAPI
)
