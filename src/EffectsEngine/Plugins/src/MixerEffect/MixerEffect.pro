include(../../../PluginsAPI/PluginsAPI.pri)
INCLUDEPATH +=   ../../../PluginsAPI
DEPENDPATH +=   ../../../PluginsAPI

TARGET =      VLMC_MixerEffectPlugin
TEMPLATE      = lib
CONFIG       += plugin
HEADERS      += MixerEffectPlugin.h \
                MixerEffectPluginCreator.h
SOURCES      += MixerEffectPlugin.cpp \
                MixerEffectPluginCreator.cpp
DESTDIR       = ../../lib
