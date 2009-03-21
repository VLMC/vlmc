TARGET = vlmc
DESTDIR = bin
CONFIG += debug
TEMPLATE = app
OBJECTS_DIR = build
QT += gui \
    network
SOURCES += src/main.cpp \
    src/gui/MainWindow.cpp \
    src/gui/LibraryWidget.cpp \
    src/gui/Timeline.cpp
HEADERS += src/gui/MainWindow.h \
    src/gui/LibraryWidget.h \
    src/gui/Timeline.h
FORMS += src/gui/ui/MainWindow.ui
RESOURCES += 
