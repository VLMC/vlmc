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
TRANSLATIONS = ts/vlmc_es.ts \
    ts/vlmc_fr.ts \
    ts/vlmc_sv.ts
RESOURCES += ressources.qrc
DEPENDPATH += src
INCLUDEPATH += src

# Pri files include
include(src/src.pri)

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
