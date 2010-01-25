PROJECT(GreenFilterEffectPlugin)

INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(
    ${QT_INCLUDE_DIR}
    ../../../PluginsAPI
)

SET(SOURCES_CPP
    GreenFilterEffectPlugin.cpp
    GreenFilterEffectPluginCreator.cpp
    ../../../PluginsAPI/LightVideoFrame.cpp
)

SET(SOURCES_H
    GreenFilterEffectPluginCreator.h
)

QT4_WRAP_CPP(SOURCES_H_MOC ${SOURCES_H})

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

ADD_LIBRARY(GreenFilterEffectPlugin SHARED ${SOURCES_CPP} ${SOURCES_H_MOC})
ADD_DEPENDENCIES(GreenFilterEffectPlugin vlmc)

TARGET_LINK_LIBRARIES(GreenFilterEffectPlugin ${QT_LIBRARIES})

INSTALL(TARGETS GreenFilterEffectPlugin LIBRARY DESTINATION ${VLMC_LIB_DIR})
