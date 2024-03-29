cmake_minimum_required (VERSION 3.22)
project (ProceduralPlanet)
include(ImportQGLViewer.cmake)
find_package(CGAL REQUIRED COMPONENTS Core)
find_package(TBB REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_BUILD_TYPE "Release")

set(PROJECT_SOURCES
    Planet.hpp
    PlanetDockWidget.hpp
    PlanetViewer.hpp
    Plate.hpp
    Mesh.hpp
    Window.hpp
    Noise.hpp
    Planet.cpp
    PlanetDockWidget.cpp
    PlanetViewer.cpp
    Window.cpp
    Noise.cpp
    Main.cpp
)

set(PROJECT_SHADERS
    GLSL/shaders/ocean.vert
    GLSL/shaders/ocean.frag
    GLSL/shaders/planet.frag
    GLSL/shaders/planet.vert
    GLSL/shaders/simple.frag
    GLSL/shaders/simple.vert
    GLSL/shaders/skybox.frag
    GLSL/shaders/skybox.vert
)

set(PROJECT_RESOURCES
    res/skybox/back.jpg
    res/skybox/bottom.jpg
    res/skybox/front.jpg
    res/skybox/left.jpg
    res/skybox/right.jpg
    res/skybox/top.jpg
    res/icons/ocean.png
    res/icons/continent.png
    res/snow_02_diff_2k.jpg
    res/rocks_ground_05_diff_2k.jpg
    res/coast_sand_rocks_02_diff_2k.jpg
    res/coast_sand_01_diff_2k.jpg
    icon.svg
)

add_compile_options(
    -Wall -pedantic -O3 -ftree-vectorize -pipe -fpic -std=c++20 -march=native
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PROJECT_SOURCES} ${PROJECT_SHADERS})
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_SHADERS})
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)
target_link_libraries(${PROJECT_NAME} QGLViewer CGAL::CGAL CGAL::CGAL_Core TBB::tbb Threads::Threads ${GLUT_LIBRARY} ${OPENGL_LIBRARY})

#Copy ressources next to executable
foreach(RES ${PROJECT_SHADERS} ${PROJECT_RESOURCES})
	configure_file(${RES} ${RES} COPYONLY)
endforeach()
