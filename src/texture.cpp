/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* texture.cpp
    Contains the implementation of TextureLoader class. */

#include "texture.h"

#include "application.h"

#include <SDL/SDL_image.h>

#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;


unsigned int TextureLoader::loadTexture(const std::string &fileName,
                                        int minFilter,
                                        int magFilter,
                                        bool alpha)
{
  SDL_Surface *textureImage = IMG_Load(fileName.c_str());

  if (!textureImage)
  {
    Application::instance()->print("TextureLoader",
      "Loading texture '" + fileName + string("' failed: ") + IMG_GetError());
    Application::instance()->quit(1);
    return 0;
  }

  int format = 0;
  if (alpha)
    format = GL_RGBA;
  else
    format = GL_RGB;

  unsigned int textureID = 0;
  glGenTextures(1, &textureID);

  glBindTexture(GL_TEXTURE_2D, textureID);

  if ((minFilter == GL_NEAREST) or (minFilter == GL_LINEAR))
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureImage->w, textureImage->h,
                 0, format, GL_UNSIGNED_BYTE, textureImage->pixels);
  }
  else
  {
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, textureImage->w,
                      textureImage->h, format, GL_UNSIGNED_BYTE,
                      textureImage->pixels);
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

  SDL_FreeSurface(textureImage);

  return textureID;
}
