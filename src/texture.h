/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* texture.h
    Zawiera klasę TextureLoader, która umożliwia ładowanie
    tekstur z plików. */

#pragma once

#include "config.h"

#include <string>

class TextureLoader
{
  public:
    static unsigned int loadTexture(const std::string &fileName, int minFilter,
                                    int magFilter, bool alpha = false);
};
