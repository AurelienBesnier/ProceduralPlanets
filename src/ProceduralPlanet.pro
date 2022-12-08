QT += opengl widgets gui xml
TEMPLATE = app
TARGET = ProceduralPlanet
MOC_DIR = ./moc
OBJECTS_DIR = ./obj
DEPENDPATH += ./GLSL
INCLUDEPATH += ./GLSL
SOURCES += Main.cpp \
    Planet.cpp \
    PlanetDockWidget.cpp \
    PlanetViewer.cpp \
    Window.cpp
HEADERS += \
    Planet.hpp \
    PlanetDockWidget.hpp \
    PlanetViewer.hpp \
    Window.hpp
LIBS = -lQGLViewer-qt5 \
    -lglut \
    -lGLU \
    -lmpfr \
    -lgmp \
    -lm

OTHER_FILES = ./GLSL/shaders/*
