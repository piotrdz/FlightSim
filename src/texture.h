/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* texture.h
    Contains the class TextureLoader, which enables loading of textures
    from files. */

#pragma once

#include "config.h"

#include <string>

class TextureLoader
{
  public:
    static unsigned int loadTexture(const std::string &fileName, int minFilter,
                                    int magFilter, bool alpha = false);
};
