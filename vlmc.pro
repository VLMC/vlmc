TARGET = vlmc
DESTDIR = bin
CONFIG += debug
TEMPLATE = app
VERSION = 0.0.1
OBJECTS_DIR = build
MOC_DIR = build/moc
UI_DIR = build/ui
QMAKE_EXT_H = .h \
    .hpp
INCLUDEPATH += build/moc \
    build/ui \
    $$[VLMC_ADDITIONAL_INCLUDEPATH]
QT += gui \
    network \
    svg \
    xml

# Pri files include
include(src/src.pri)
include(src/Actions/actions.pri)
include(src/Commands/commands.pri)
include(src/Configuration/configuration.pri)
include(src/EffectsEngine/EffectsEngine.pri)
include(src/GUI/GUI.pri)
include(src/GUI/import/import.pri)
include(src/GUI/library/library.pri)
include(src/GUI/settings/settings.pri)
include(src/GUI/widgets/widgets.pri)
include(src/GUI/wizard/wizard.pri)
include(src/LibVLCpp/LibVLCpp.pri)
include(src/Library/Library.pri)
include(src/Media/Media.pri)
include(src/Metadata/Metadata.pri)
include(src/Project/projectManager.pri)
include(src/Renderer/Renderer.pri)
include(src/Tools/Tools.pri)
include(src/Workflow/Workflow.pri)
INCLUDEPATH += src/LibVLCpp \
    src/GUI \
    src/GUI/widgets \
    src/GUI/library \
    src/GUI/import \
    src/GUI/settings \
    src/GUI/wizard \
    src/Tools \
    src/Renderer \
    src/Metadata \
    src/Commands \
    src/Workflow \
    src/Library \
    src/Media \
    src/Project \
    src/EffectsEngine \
    src/Configuration \
    src/EffectsEngine \
    src/Actions \
    src
DEPENDPATH += src/LibVLCpp \
    src/GUI \
    src/GUI/import \
    src/GUI/library \
    src/GUI/settings \
    src/GUI/widgets \
    src/GUI/wizard \
    src/Tools \
    src/Renderer \
    src/Metadata \
    src/Commands \
    src/Workflow \
    src/Library \
    src/Media \
    src/Project \
    src/EffectsEngine \
    src/Configuration \
    src/EffectsEngine \
    src/Actions \
    src
TRANSLATIONS = ts/vlmc_es.ts \
    ts/vlmc_fr.ts \
    ts/vlmc_sv.ts
RESOURCES += ressources.qrc

# QMAKE_CFLAGS+=-pg
# QMAKE_CXXFLAGS+=-pg
# QMAKE_LFLAGS+=-pg
# QMAKE_CXXFLAGS += -W -Wall -Wold-style-cast
LIBS += -L/usr/local/lib \
    -lvlc \
    $$[VLMC_ADDITIONAL_LIBS]
DEFINES += VLMC_VERSION="$$VERSION"
CODECFORTR = UTF-8
include(locale.pri)
