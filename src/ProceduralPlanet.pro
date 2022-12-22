QT += opengl widgets gui xml
QMAKE_CXXFLAGS = -O3 -std=c++20 -march=znver3
QMAKE_CXXFLAGS_DEBUG -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O2
CCFLAG -= -02
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
    Plate.hpp \
    Window.hpp
LIBS = -lQGLViewer-qt5 \
    -lglut \
    -lGLU \
    -lmpfr \
    -lgmp \
    -lm \
    -ltcmalloc

OTHER_FILES = ./GLSL/shaders/*
