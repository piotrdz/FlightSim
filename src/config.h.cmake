/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

#pragma once

// Makro ustawiane/nie ustawiane przez CMake
#cmakedefine DEBUG

// Ustawienie NDEBUG dla assert
#ifndef DEBUG
#define NDEBUG
#endif

// Potrzebne na Windows
#if defined(WIN32) || defined(_WIN32)
#undef GL_VERSION_1_5
#include <SDL/SDL_opengl.h>
#endif