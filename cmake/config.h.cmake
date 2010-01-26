#ifndef CONFIG_H
#define CONFIG_H

/* With crash button (tools->crash) */
#cmakedefine WITH_CRASHBUTTON

/* With crash handler */
#cmakedefine WITH_CRASHHANDLER

/* With crash handler GUI */
#cmakedefine WITH_CRASHHANDLER_GUI

/* Absolute path to plugins */
#cmakedefine VLMC_EFFECTS_DIR "@VLMC_EFFECTS_DIR@"

/* Host computer name */
#cmakedefine HOSTNAME "@HOSTNAME@"

#endif //CONFIG_H
