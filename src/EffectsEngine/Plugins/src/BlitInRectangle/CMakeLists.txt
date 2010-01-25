PROJECT(BlitInRectangleEffectPlugin)

INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(
    ${QT_INCLUDE_DIR}
    ../../../PluginsAPI
)

SET(SOURCES_CPP
    BlitInRectangleEffectPlugin.cpp
    BlitInRectangleEffectPluginCreator.cpp
    ../../../PluginsAPI/LightVideoFrame.cpp
)

SET(SOURCES_H
    BlitInRectangleEffectPluginCreator.h
)

QT4_WRAP_CPP(SOURCES_H_MOC ${SOURCES_H})

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

ADD_LIBRARY(BlitInRectangleEffectPlugin SHARED ${SOURCES_CPP} ${SOURCES_H_MOC})
ADD_DEPENDENCIES(BlitInRectangleEffectPlugin vlmc)

TARGET_LINK_LIBRARIES(BlitInRectangleEffectPlugin ${QT_LIBRARIES})

INSTALL(TARGETS BlitInRectangleEffectPlugin LIBRARY DESTINATION ${VLMC_EFFECTS_DIR})
