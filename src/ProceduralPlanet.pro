QT += xml \
    widgets
QT += opengl
TARGET = ProceduralPlanet
MOC_DIR = ./moc
OBJECTS_DIR = ./obj
DEPENDPATH += ./GLSL
INCLUDEPATH += ./GLSL
SOURCES += Main.cpp \
    Planet.cpp \
    PlanetDockWidget.cpp \
    PlanetViewer.cpp \
    PlanetViewer2.cpp \
    Window.cpp
HEADERS += \
    Planet.hpp \
    PlanetDockWidget.hpp \
    PlanetViewer.hpp \
    PlanetViewer2.hpp \
    Window.hpp
INCLUDEPATH = ./GLSL
LIBS = -lQGLViewer-qt5 \
    -lglut \
    -lGLU \
    -lm

OTHER_FILES = ./GLSL/shaders/*
