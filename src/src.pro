TEMPLATE = app
TARGET = vlmc
DESTDIR = ../bin
CONFIG += debug
VERSION = 0.0.1
OBJECTS_DIR = ../build
MOC_DIR = ../build/moc
UI_DIR = ../build/ui
QMAKE_EXT_H = .h \
    .hpp
INCLUDEPATH += ../build/moc \
    ../build/ui \
    $$[VLMC_ADDITIONAL_INCLUDEPATH]
QT += gui \
    network \
    svg \
    xml

SOURCES += main.cpp

# Pri files include
include(Actions/actions.pri)
include(Commands/commands.pri)
include(Configuration/configuration.pri)
include(EffectsEngine/EffectsEngine.pri)
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
INCLUDEPATH += LibVLCpp \
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
    Configuration \
    EffectsEngine \
    Actions
DEPENDPATH += LibVLCpp \
    GUI \
    GUI/import \
    GUI/library \
    GUI/settings \
    GUI/widgets \
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
    Configuration \
    EffectsEngine \
    Actions
TRANSLATIONS =
    ../ts/vlmc_cs.ts \
    ../ts/vlmc_es.ts \
    ../ts/vlmc_fr.ts \
    ../ts/vlmc_jp.ts \
    ../ts/vlmc_sv.ts
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
# QMAKE_CFLAGS+=-pg
# QMAKE_CXXFLAGS+=-pg
# QMAKE_LFLAGS+=-pg
# QMAKE_CXXFLAGS += -W -Wall -Wold-style-cast
