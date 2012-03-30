/***************************************************************************
 *   Copyright (C) 2011-2012 by Piotr Dziwinski                            *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

#pragma once

// Macro set/not set by CMake
#cmakedefine DEBUG

// NDEBUG for assert()
#ifndef DEBUG
#define NDEBUG
#endif

// Needed on Windows
#if defined(WIN32) || defined(_WIN32)
#undef GL_VERSION_1_5
#include <SDL/SDL_opengl.h>
#endif