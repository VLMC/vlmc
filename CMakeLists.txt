#Forking CMakeQt by Rohit Yadav<rohityadav89@gmail.com>
#CMake Build System for Qt Project: VLMC
#Edit as per needs...

#Project Info
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "1")
SET(COMPLETE_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})
#Codename of the Project :-)
SET(RELEASE_NAME "Alpha")
# version: 0.0.1
SET(PROJECT_VERSION ${COMPLETE_VERSION})
#Name of the project
PROJECT(vlmc${PROJECT_VERSION})

SITE_NAME(HOSTNAME)

#CMake Settings
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE OFF)
# set path to additional CMake modules
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Configuring build type
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
    MESSAGE("Setting build type to '${CMAKE_BUILD_TYPE}'")
ELSE(NOT CMAKE_BUILD_TYPE)
    MESSAGE("Build type set to '${CMAKE_BUILD_TYPE}'")
ENDIF(NOT CMAKE_BUILD_TYPE)

IF(WIN32)
    # Building in release for WIN32
    set(CMAKE_BUILD_TYPE "Release")
    SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE QT_NO_DEBUG)
ENDIF(WIN32)

# Optionally allow embedding libvlc into the binary as
# VLMC requires libVLC
#SET (WITH_LIBVLC FALSE CACHE BOOL "Compile LibVLC in VLMC")

SET(WITH_CRASHBUTTON FALSE CACHE BOOL "Enable the crash button")

FIND_PACKAGE(LIBVLC)
  IF (NOT LIBVLC_FOUND)
    MESSAGE (SEND_ERROR "LibVLC dependency was not found (please get libvlc)!")
  ENDIF (NOT LIBVLC_FOUND)

INCLUDE_DIRECTORIES(${LIBVLC_INCLUDE_DIR})

# search for Qt4
FIND_PACKAGE(Qt4 4.5.1 COMPONENTS QtCore QtGui QtSvg QtXml REQUIRED )

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

# layout of VLMC installation
# (default values are platform-specific)
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/effects)

# Copy the sh launcher
IF (NOT WIN32)
    CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/cmake/vlmc.sh" "${CMAKE_CURRENT_BINARY_DIR}/vlmc" COPYONLY)
ENDIF(NOT WIN32)

# common for MAC and UNIX
SET (DEFAULT_BIN_SUBDIR     bin)
SET (DEFAULT_LIB_SUBDIR     lib)
SET (DEFAULT_DATA_SUBDIR    share)
SET (DEFAULT_SRC_SUBDIR     src)
SET (DEFAULT_INCLUDE_SUBDIR include)

SET (LIB_SUFFIX          ""                             CACHE STRING "Define lib directory suffix (32/64)")
SET (VLMC_BIN_SUBDIR     "${DEFAULT_BIN_SUBDIR}"        CACHE STRING "Subdirectory where executables will be installed")
SET (VLMC_LIB_SUBDIR     "${DEFAULT_LIB_SUBDIR}"        CACHE STRING "Subdirectory where libraries will be installed")
SET (VLMC_DATA_SUBDIR    "${DEFAULT_DATA_SUBDIR}"       CACHE STRING "Subdirectory where VLMC data will be installed")
SET (VLMC_SRC_SUBDIR     "${DEFAULT_SRC_SUBDIR}"        CACHE STRING "Subdirectory where VLMC source is")
SET (VLMC_INCLUDE_SUBDIR "${DEFAULT_INCLUDE_SUBDIR}"    CACHE STRING "Subdirectory where header files will be installed")

# append the suffix (if any) to the lib directory
SET (VLMC_LIB_SUBDIR "${VLMC_LIB_SUBDIR}${LIB_SUFFIX}")

# full paths for the installation
SET (VLMC_BIN_DIR     ${CMAKE_INSTALL_PREFIX}/${VLMC_BIN_SUBDIR})
SET (VLMC_LIB_DIR     ${CMAKE_INSTALL_PREFIX}/${VLMC_LIB_SUBDIR})
SET (VLMC_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/${VLMC_INCLUDE_SUBDIR})
SET (VLMC_DATA_DIR    ${CMAKE_INSTALL_PREFIX}/${VLMC_DATA_SUBDIR}/vlmc/)
SET (VLMC_DOC_DIR     ${CMAKE_INSTALL_PREFIX}/${VLMC_DATA_SUBDIR}/doc/vlmc/)
SET (VLMC_EFFECTS_DIR ${CMAKE_INSTALL_PREFIX}/${VLMC_LIB_SUBDIR}/vlmc/effects/)
SET (VLMC_DESKTOP_DIR ${CMAKE_INSTALL_PREFIX}/${VLMC_DATA_SUBDIR}/applications/)

# manual page - makes sense only on unix systems
IF (UNIX)
  SET (DEFAULT_MANUAL_SUBDIR  man)
  SET (VLMC_MANUAL_SUBDIR  ${DEFAULT_MANUAL_SUBDIR}  CACHE STRING "Subdirectory where manual files will be installed")
  MARK_AS_ADVANCED (VLMC_MANUAL_SUBDIR)
  SET (VLMC_MANUAL_DIR  ${CMAKE_INSTALL_PREFIX}/${VLMC_MANUAL_SUBDIR})
ENDIF (UNIX)

SUBDIRS(ts)
SUBDIRS(src)

# Copy stuff to doc subdirectory
INSTALL (FILES AUTHORS COPYING INSTALL NEWS README TRANSLATORS
         DESTINATION ${VLMC_DOC_DIR})

# Copy .desktop file
INSTALL (FILES share/vlmc.desktop DESTINATION ${VLMC_DESKTOP_DIR})

# Copy the share directory
INSTALL (DIRECTORY share/ DESTINATION ${VLMC_DATA_DIR})

#############################################################
# Enable packaging

INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "VLMC")
SET(CPACK_PACKAGE_VENDOR "VLMC Team")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "VLMC ${COMPLETE_VERSION}")
IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backslashes.
  SET(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/share\\\\vlmc.png")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "\\\\vlmc.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} VLMC")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\vlmc.org")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\vlmc.org")
  SET(CPACK_NSIS_CONTACT "vlmc-devel@vlmc.org")
  SET(CPACK_NSIS_MODIFY_PATH ON)

#  SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "  !include \\\"${CMAKE_SOURCE_DIR}\\\\win_build\\\\extra.nsh\\\"")
ELSE(WIN32 AND NOT UNIX)
  #SET(CPACK_STRIP_FILES "VLMC")
  #SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "vlmc" "VLMC")
INCLUDE(CPack)


