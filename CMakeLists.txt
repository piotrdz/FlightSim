cmake_minimum_required(VERSION 2.8)

project(flightsim C CXX)

find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(Gettext REQUIRED)

find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
  add_custom_target(doc ${DOXYGEN_EXECUTABLE} Doxyfile
    COMMENT "Generowanie dokumentacji API w Doxygenie" VERBATIM)
endif(DOXYGEN_FOUND)

option(DEBUG "Debug" ON)

set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g -O0")

configure_file(src/config.h.cmake src/config.h)

set(SOURCES
  src/main.cpp
  src/common.cpp
  src/object.cpp
  src/widget.cpp
  src/application.cpp
  src/render.cpp
  src/fontengine.cpp
  src/filemanager.cpp
  src/texture.cpp
  src/decorator.cpp
  src/label.cpp
  src/menu.cpp
  src/dialog.cpp
  src/controls.cpp
  src/console.cpp
  src/bindings.cpp
  src/settings.cpp
  src/settingsdialog.cpp
  src/fractal.cpp
  src/map.cpp
  src/rotation.cpp
  src/model.cpp
  src/mapdialog.cpp
  src/gamedialog.cpp
  src/bullet.cpp
  src/player.cpp
  src/simulation.cpp)

set(LIBS ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY} ${OPENGL_LIBRARY})

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(LIBS ${LIBS} SDLmain.a)
endif(CMAKE_SYSTEM_NAME MATCHES "Windows")

add_executable(bin/flightsim ${SOURCES})

target_link_libraries(bin/flightsim ${LIBS})

add_subdirectory(po)
