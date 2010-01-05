include(../../../PluginsAPI/PluginsAPI.pri)
INCLUDEPATH +=   ../../../PluginsAPI
DEPENDPATH +=   ../../../PluginsAPI

TARGET =      VLMC_BlitInRectangleEffectPlugin
TEMPLATE      = lib
CONFIG       += plugin
HEADERS      += BlitInRectangleEffectPlugin.h \
                BlitInRectangleEffectPluginCreator.h
SOURCES      += BlitInRectangleEffectPlugin.cpp \
                BlitInRectangleEffectPluginCreator.cpp
DESTDIR       = ../../lib
