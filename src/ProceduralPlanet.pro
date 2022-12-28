QT += opengl widgets gui xml
QMAKE_CXXFLAGS = -O3 -std=c++20 -march=znver1 -pedantic
QMAKE_CXXFLAGS_DEBUG -= -O2 -O1
QMAKE_CXXFLAGS_RELEASE -= -O2 -O1
CCFLAG -= -O2 -O1
CCFLAG += -O3 -std=c++20 -march=x86-64
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
    Mesh.hpp \
    Window.hpp
LIBS = -lQGLViewer-qt5 \
    -lglut \
    -lGLU \
    -lmpfr \
    -lgmp \
    -lm \
    -ltcmalloc

OTHER_FILES = ./GLSL/shaders/*

DISTFILES += \
    GLSL/shaders/ocean.frag \
    GLSL/shaders/ocean.vert \
    GLSL/shaders/simple.frag \
    GLSL/shaders/simple.vert \
    GLSL/shaders/planet.vert \
    GLSL/shaders/planet.frag
