HEADERS	+=	vlmc.h

SOURCES	+=	main.cpp


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
