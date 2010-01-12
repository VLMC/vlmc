# Basic settings
TEMPLATE = app
TARGET = vlmc
CONFIG += debug
VERSION = 0.0.1

# Build paths configurations
DESTDIR = ../bin
OBJECTS_DIR = ../build
MOC_DIR = ../build/moc
UI_DIR = ../build/ui

# Make both .h and .hpp C++ headers extensions
QMAKE_EXT_H = .h \
    .hpp

QT +=   gui \
        network \
        svg \
        xml

SOURCES += main.cpp

# Pri files include
include(Actions/actions.pri)
include(Commands/commands.pri)
include(Configuration/configuration.pri)
include(EffectsEngine/EffectsEngine.pri)
include(EffectsEngine/PluginsAPI/PluginsAPI.pri)
include(Gui/Gui.pri)
include(Gui/import/import.pri)
include(Gui/library/library.pri)
include(Gui/settings/settings.pri)
include(Gui/widgets/widgets.pri)
include(Gui/wizard/wizard.pri)
include(LibVLCpp/LibVLCpp.pri)
include(Library/Library.pri)
include(Media/Media.pri)
include(Metadata/Metadata.pri)
include(Project/projectManager.pri)
include(Renderer/Renderer.pri)
include(Tools/Tools.pri)
include(Workflow/Workflow.pri)


VLMC_DIRS = LibVLCpp \
    Gui \
    Gui/widgets \
    Gui/library \
    Gui/import \
    Gui/settings \
    Gui/wizard \
    Tools \
    Renderer \
    Metadata \
    Commands \
    Workflow \
    Library \
    Media \
    Project \
    EffectsEngine \
    EffectsEngine/PluginsAPI \
    Configuration \
    EffectsEngine \
    Actions

INCLUDEPATH += $$VLMC_DIRS \
            ../build/moc \
            ../build/ui \
            $$[VLMC_ADDITIONAL_INCLUDEPATH]

DEPENDPATH += $$VLMC_DIRS

# Add translations here, it will be automatically build by the next line.
TRANSLATIONS_LIST = es fr ja_JP sv pt_BR sk it uk cs
# Add every translation in the TRANSLATION variable
for(lang, TRANSLATIONS_LIST):exists( ../ts/vlmc_$${lang}.ts ):TRANSLATIONS += ../ts/vlmc_$${lang}.ts

RESOURCES += ../ressources.qrc

exists( ../ts/*.qm ) {
    RESOURCES += ../ressources-ts.qrc
}

LIBS += -L/usr/local/lib \
        -lvlc \
        $$[VLMC_ADDITIONAL_LIBS]
DEFINES += VLMC_VERSION="$$VERSION"

CODECFORTR = UTF-8
include(../locale.pri)

exists( src.user.pro ) {
    include( src.user.pro )
}

