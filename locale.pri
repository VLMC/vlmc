!isEmpty(TRANSLATIONS)
{
    isEmpty(QMAKE_LRELEASE)
    {
        win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
        else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
    }
    isEmpty(TS_DIR):TS_DIR = Translations

    TSQM.name = lrelease ${QMAKE_FILE_IN}
    TSQM.input = TRANSLATIONS
    TSQM.output = $$TS_DIR/${QMAKE_FILE_BASE}.qm
    TSQM.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN}
    TSQM.CONFIG = no_link
    QMAKE_EXTRA_COMPILERS += TSQM
    PRE_TARGETDEPS += compiler_TSQM_make_all
 }
