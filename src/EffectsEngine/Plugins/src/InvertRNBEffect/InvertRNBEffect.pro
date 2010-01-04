include(../../../PluginsAPI/PluginsAPI.pri)
INCLUDEPATH +=   ../../../PluginsAPI
DEPENDPATH +=   ../../../PluginsAPI

TARGET =      VLMC_InvertRNBEffectPlugin
TEMPLATE      = lib
CONFIG       += plugin
HEADERS      += InvertRNBEffectPlugin.h \
                InvertRNBEffectPluginCreator.h
SOURCES      += InvertRNBEffectPlugin.cpp \
                InvertRNBEffectPluginCreator.cpp
DESTDIR       = ../../lib
