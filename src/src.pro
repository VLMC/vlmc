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
include(GUI/GUI.pri)
include(GUI/import/import.pri)
include(GUI/library/library.pri)
include(GUI/settings/settings.pri)
include(GUI/widgets/widgets.pri)
include(GUI/wizard/wizard.pri)
include(LibVLCpp/LibVLCpp.pri)
include(Library/Library.pri)
include(Media/Media.pri)
include(Metadata/Metadata.pri)
include(Project/projectManager.pri)
include(Renderer/Renderer.pri)
include(Tools/Tools.pri)
include(Workflow/Workflow.pri)


VLMC_DIRS = LibVLCpp \
    GUI \
    GUI/widgets \
    GUI/library \
    GUI/import \
    GUI/settings \
    GUI/wizard \
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

TRANSLATIONS = ../ts/vlmc_cs.ts \
               ../ts/vlmc_es.ts \
               ../ts/vlmc_fr.ts \
               ../ts/vlmc_jp.ts \
               ../ts/vlmc_sv.ts \
               ../ts/vlmc_pt_BR.ts
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

