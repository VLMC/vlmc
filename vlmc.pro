TARGET = vlmc
DESTDIR = bin
CONFIG += debug
TEMPLATE = app
QT += gui \
    network
SOURCES += src/main.cpp \
    src/gui/MainWindow.cpp \
    src/gui/LibraryWidget.cpp
HEADERS += src/gui/MainWindow.h \
    src/gui/LibraryWidget.h
FORMS += src/gui/ui/MainWindow.ui
RESOURCES += 
