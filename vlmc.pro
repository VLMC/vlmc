!contains(QT_MAJOR_VERSION, 4) {
    error( "Unsupported Qt version $$QT_VERSION found. Requires at least Qt 4.5" )
}

!contains( QT_MINOR_VERSION, 5 ) {
    !contains( QT_MINOR_VERSION, 6 ) {
        error( "Unsupported Qt version $$QT_VERSION found. Requires at least Qt 4.5" )
    }
}

TEMPLATE = subdirs
SUBDIRS =   src \
            launcher \
            src/EffectsEngine/Plugins/src/src.pro
CONFIG += ordered

DEPENDPATH += src \
            launcher \
            src/EffectsEngine/Plugins/src/src.pro

