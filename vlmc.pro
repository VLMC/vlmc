TARGET = vlmc
DESTDIR = bin
CONFIG += debug
TEMPLATE = app
QT += gui \
    network
SOURCES += src/main.cpp \
    src/gui/MainWindow.cpp
HEADERS += src/gui/MainWindow.h
FORMS += src/gui/ui/MainWindow.ui
RESOURCES += 
