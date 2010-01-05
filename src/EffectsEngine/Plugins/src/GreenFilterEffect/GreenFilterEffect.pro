include(../../../PluginsAPI/PluginsAPI.pri)
INCLUDEPATH +=   ../../../PluginsAPI
DEPENDPATH +=   ../../../PluginsAPI

TARGET =      VLMC_GreenFilterEffectPlugin
TEMPLATE      = lib
CONFIG       += plugin
HEADERS      += GreenFilterEffectPlugin.h \
                GreenFilterEffectPluginCreator.h
SOURCES      += GreenFilterEffectPlugin.cpp \
                GreenFilterEffectPluginCreator.cpp
DESTDIR       = ../../lib
