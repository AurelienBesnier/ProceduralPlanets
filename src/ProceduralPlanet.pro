QT += xml
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
    Window.cpp
HEADERS += Window.h \
    Planet.h \
    PlanetDockWidget.h \
    PlanetViewer.h \
    Vec3D.h
INCLUDEPATH = ./GLSL
LIBS = -lQGLViewer-qt5 \
    -lglut \
    -lGLU

OTHER_FILES = ./GLSL/shaders/*
