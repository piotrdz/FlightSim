/***************************************************************************
 *   Copyright (C) 2011-2012 by Piotr Dziwinski                            *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* fontengine.cpp
    Contains the implementation of classes and structs from fontengine.h. */

#include "fontengine.h"

#include "application.h"
#include "render.h"
#include "events.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include <sstream>
#include <cmath>
#include <cassert>

using namespace std;


const SDL_Color COLOR_WHITE = {255, 255, 255, 0};
const double LOG_2 = log(2.0);


FontManager* FontManager::_instance = NULL;

FontManager::FontManager() : Object("FontManager")
{
  assert(_instance == NULL);
  _instance = this;

  if (!TTF_WasInit())
  {
    if (TTF_Init() != 0)
    {
      print("TTF_Init error: " + string(TTF_GetError()));
      Application::instance()->quit(1);
    }
  }

  _referenceWindowSize = Application::instance()->windowSettings().size;
}

FontManager::~FontManager()
{
  for (FontMapIterator it = _fontMap.begin(); it != _fontMap.end(); ++it)
  {
    delete (*it).second;
    (*it).second = NULL;
  }

  if (TTF_WasInit())
    TTF_Quit();

  _instance = NULL;
}

Font* FontManager::getFont(const string &fontPath, int pointSize)
{
  return getFont(FontOptions(fontPath, pointSize));
}

Font* FontManager::getFont(const FontOptions &options)
{
   FontMapIterator it = _fontMap.find(options);

  if (it != _fontMap.end())
    return (*it).second;

  FontOptions actualOptions = options;
  float multiply = Application::instance()->windowSettings().size.diagonal() /
                   _referenceWindowSize.diagonal();
  actualOptions.pointSize = (int)(multiply * options.pointSize);

   Font *font = new Font(actualOptions);
  _fontMap.insert(make_pair(options, font));

  return font;
}

void FontManager::windowResized(WindowResizeEvent *e)
{
  float multiply = e->newSize().diagonal() /
                   _referenceWindowSize.diagonal();

  for (FontMapIterator it = _fontMap.begin(); it != _fontMap.end(); ++it)
  {
    Font *f = (*it).second;
    FontOptions newOptions = (*it).first;
    newOptions.pointSize = (int)(multiply * newOptions.pointSize);
    if (newOptions.pointSize != f->options().pointSize)
    {
      f->changeOptions(newOptions);
      f->clearCache();
    }
  }

  FontResizeEvent event;
  Render::instance()->sendEvent(&event);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int Font::_staticTextId = 0;

Font::Font(const FontOptions &pOptions) : Object(genericName("Font")),
  _fontOptions(pOptions)
{
  _font = NULL;
  if (!TTF_WasInit())
  {
    if (TTF_Init() != 0)
    {
      print("TTF_Init error: " + string(TTF_GetError()));
      Application::instance()->quit(1);
    }
  }

  _font = TTF_OpenFont(_fontOptions.fileName.c_str(),
                       _fontOptions.pointSize);
  if (!_font)
  {
    print("Could not open font: " + string(TTF_GetError()));
    Application::instance()->quit(1);
  }
}

Font::~Font()
{
  TTF_CloseFont(_font);
  _font = NULL;
}

void Font::clearCache()
{
  _asciiCache.clear();
  _utf8Cache.clear();
  _staticCache.clear();
}

void Font::changeOptions(const FontOptions &newOptions)
{
  _fontOptions = newOptions;

  if (_font != NULL)
    TTF_CloseFont(_font);
  _font = NULL;

  _font = TTF_OpenFont(_fontOptions.fileName.c_str(),
                       _fontOptions.pointSize);

  if (!_font)
  {
    print("Could not open font: " + string(TTF_GetError()));
    Application::instance()->quit(1);
  }
}

void Font::renderText(const string &text, const Point &location)
{
  if ((text.empty()) || (_font == NULL))
    return;

  char character = '\0';
  CachedTexture texture;
  float x = location.x;

  glDisable(GL_MULTISAMPLE);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  string::const_iterator it;
  char charText[2] = {'\0', '\0'};
  for (it = text.begin(); it != text.end(); ++it)
  {
    character = *it;
    texture = _asciiCache.search(character);

    if (texture.id == 0)
    {
      charText[0] = character;
      texture = createTexture(charText, false);
      _asciiCache.add(character, texture);
    }

    glBindTexture(GL_TEXTURE_2D, texture.id);

    glBegin(GL_QUADS);
    {
      glTexCoord2f(0.0f, 0.0f);
      glVertex2f(x, location.y);
      glTexCoord2f(1.0f, 0.0f);
      glVertex2f(x + texture.width, location.y);
      glTexCoord2f(1.0f, 1.0f);
      glVertex2f(x + texture.width, location.y + texture.height);
      glTexCoord2f(0.0f, 1.0f);
      glVertex2f(x, location.y + texture.height);
    }
    glEnd();

    x += texture.charWidth;
  }

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);

  glEnable(GL_MULTISAMPLE);
}

void Font::renderTextUTF8(const string &text, const Point &location)
{
  if ((text.empty()) || (_font == NULL))
    return;

  UTF8Char character;
  CachedTexture texture;
  float x = location.x;

  glDisable(GL_MULTISAMPLE);


  glEnable(GL_TEXTURE_2D);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  string::const_iterator it = text.begin();
  while (it != text.end())
  {
    character.reset();
    bool ascii = false;

    char c1 = *it;
    // 1-byte char (ASCII)
    if ((c1 & 0x80) == 0)
    {
      character.bytes[0] = c1;
      ascii = true;
      ++it;
    }
    // 2-byte
    else if ((c1 & 0xE0) == 0xC0)
    {
      character.bytes[0] = c1;
      character.bytes[1] = *(++it);
      ++it;
    }
    // 3-byte
    else if ((c1 & 0xF0) == 0xE0)
    {
      character.bytes[0] = c1;
      character.bytes[1] = *(++it);
      character.bytes[2] = *(++it);
      ++it;
    }
    // 4-byte
    else if ((c1 & 0xF8) == 0xF0)
    {
      character.bytes[0] = c1;
      character.bytes[1] = *(++it);
      character.bytes[2] = *(++it);
      character.bytes[3] = *(++it);
      ++it;
    }
    // Invalid char, but treated as ASCI
    else
    {
      character.bytes[0] = c1;
      ascii = true;
      ++it;
    }

    if (ascii)
      texture = _asciiCache.search(c1);
    else
      texture = _utf8Cache.search(character);

    if (texture.id == 0)
    {
      if (ascii)
      {
        texture = createTexture(character.bytes, false);
        _asciiCache.add(c1, texture);
      }
      else
      {
        texture = createTexture(character.bytes, true);
        _utf8Cache.add(character, texture);
      }
    }

    glBindTexture(GL_TEXTURE_2D, texture.id);

    glBegin(GL_QUADS);
    {
      glTexCoord2f(0.0f, 0.0f);
      glVertex2f(x, location.y);
      glTexCoord2f(1.0f, 0.0f);
      glVertex2f(x + texture.width, location.y);
      glTexCoord2f(1.0f, 1.0f);
      glVertex2f(x + texture.width, location.y + texture.height);
      glTexCoord2f(0.0f, 1.0f);
      glVertex2f(x, location.y + texture.height);
    }
    glEnd();

    x += texture.charWidth;
  }

  glDisable(GL_BLEND);

  glDisable(GL_TEXTURE_2D);

  glEnable(GL_MULTISAMPLE);
}

int Font::cacheStaticText(const string &text, bool utf8)
{
  if (_font == NULL)
    return -1;

  CachedTexture texture = createTexture(text, utf8);
  int id = _staticTextId++;
  _staticCache.add(id, texture);
  return id;
}

Size Font::staticTextMetrics(int id)
{
  CachedTexture texture = _staticCache.search(id);
  return Size(texture.charWidth, texture.charHeight);
}

void Font::renderStaticText(int id, const Point &location)
{
  if (_font == NULL)
    return;

  CachedTexture texture = _staticCache.search(id);
  if (texture.id == 0)
  {
    print("Static ID " + toString<int>(id) + " not found");
    return;
  }

  glDisable(GL_MULTISAMPLE);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glBindTexture(GL_TEXTURE_2D, texture.id);

  glBegin(GL_QUADS);
  {
    glTexCoord2f(0.0f, 0.0f); 
    glVertex2f(location.x, location.y);
    glTexCoord2f(1.0f, 0.0f); 
    glVertex2f(location.x + texture.width, location.y);
    glTexCoord2f(1.0f, 1.0f); 
    glVertex2f(location.x + texture.width, location.y + texture.height);
    glTexCoord2f(0.0f, 1.0f); 
    glVertex2f(location.x, location.y + texture.height);
  }
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);

  glEnable(GL_MULTISAMPLE);
}

Font::CachedTexture Font::createTexture(const string &text, bool utf8)
{
  CachedTexture texture;
  if (_font == NULL)
    return texture;

  SDL_Surface *textSurface = NULL;
  if (utf8)
    textSurface = TTF_RenderUTF8_Blended(_font, text.c_str(), COLOR_WHITE);
  else
    textSurface = TTF_RenderText_Blended(_font, text.c_str(), COLOR_WHITE);

  if (textSurface == NULL)
  {
    print("TTF_Render error...");
    return texture;
  }

  int w = nextPowerOfTwo(textSurface->w);
  int h = nextPowerOfTwo(textSurface->h);

  texture.width = w;
  texture.height = h;
  texture.charWidth = textSurface->w;
  texture.charHeight = textSurface->h;

  textSurface->flags = textSurface->flags & (~SDL_SRCALPHA);
  SDL_Surface *textureSurface = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000, 0x0000ff00,
                                                     0x000000ff, 0xff000000);
  SDL_BlitSurface(textSurface, NULL, textureSurface, NULL);

  glEnable(GL_TEXTURE_2D);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(1, &texture.id);

  glBindTexture(GL_TEXTURE_2D, texture.id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               textureSurface->pixels);

  glDisable(GL_TEXTURE_2D);

  SDL_FreeSurface(textSurface);
  SDL_FreeSurface(textureSurface);

  return texture;
}

int Font::nextPowerOfTwo(int x)
{
  double logbase2 = log((float)x) / LOG_2;
  return (int) (pow(2, ceil(logbase2)) + 0.5);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int FontMetrics::height()
{
  if ((_font == NULL) || (_font->_font == NULL))
    return -1;

  return TTF_FontLineSkip(_font->_font);
}

int FontMetrics::width(const string &text)
{
  if ((_font == NULL) || (_font->_font == NULL))
    return -1;

  int width = 0;
  int height = 0;

  TTF_SizeText(_font->_font, text.c_str(), &width, &height);

  return width;
}

int FontMetrics::widthUTF8(const string &text)
{
  if ((_font == NULL) || (_font->_font == NULL))
    return -1;

  int width = 0;
  int height = 0;

  TTF_SizeUTF8(_font->_font, text.c_str(), &width, &height);

  return width;
}

Size FontMetrics::textSize(const string &text)
{
  if ((_font == NULL) || (_font->_font == NULL))
    return Size(-1, -1);

  int width = 0;
  int height = 0;
  TTF_SizeText(_font->_font, text.c_str(), &width, &height);
  return Size(width, height);
}

Size FontMetrics::textSizeUTF8(const string &text)
{
  if ((_font == NULL) || (_font->_font == NULL))
    return Size(-1, -1);

  int width = 0;
  int height = 0;
  TTF_SizeUTF8(_font->_font, text.c_str(), &width, &height);
  return Size(width, height);
}
