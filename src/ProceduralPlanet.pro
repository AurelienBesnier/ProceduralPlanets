QT += opengl xml widgets gui
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
INCLUDEPATH = ./GLSL
LIBS = -lQGLViewer-qt5 \
    -lglut \
    -lGLU \
    -lm

OTHER_FILES = ./GLSL/shaders/*
