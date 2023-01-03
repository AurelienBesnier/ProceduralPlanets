cmake_minimum_required (VERSION 3.22)
project (ProceduralPlanet)
include(ImportQGLViewer.cmake)
find_package(CGAL REQUIRED COMPONENTS Core)
find_package(TBB REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

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
    icon.svg
)

add_compile_options(
    -Wall -pedantic -O3 -std=c++20 -march=native
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PROJECT_SOURCES} ${PROJECT_SHADERS})
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_SHADERS})
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)
target_link_libraries(${PROJECT_NAME} QGLViewer CGAL::CGAL CGAL::CGAL_Core TBB::tbb Threads::Threads)

#Copy ressources next to executable
foreach(RES ${PROJECT_SHADERS} ${PROJECT_RESOURCES})
	configure_file(${RES} ${RES} COPYONLY)
endforeach()
