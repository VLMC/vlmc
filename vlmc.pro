TARGET = vlmc
DESTDIR = bin
CONFIG += debug
TEMPLATE = app
OBJECTS_DIR = build
QT += gui \
    network
SOURCES += src/main.cpp \
    src/gui/MainWindow.cpp \
    src/gui/LibraryWidget.cpp
    src/LibVLCpp/VLCException.cpp \
    src/LibVLCpp/VlmManager.cpp \
    src/LibVLCpp/VLCInstance.cpp \
    src/LibVLCpp/VlmMedia.cpp
HEADERS += src/gui/MainWindow.h \
    src/gui/LibraryWidget.h
    src/LibVLCpp/VLCpp.hpp \
    src/LibVLCpp/VLCException.h \
    src/LibVLCpp/VlmManager.h \
    src/LibVLCpp/VLCInstance.h \
    src/LibVLCpp/VlmMedia.h
FORMS += src/gui/ui/MainWindow.ui
RESOURCES += 
LIBS = -L/usr/local/lib \
    -lvlc
