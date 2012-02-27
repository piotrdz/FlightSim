/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* fontengine.h
    Contains the classes and structs comprising the text rendering engine. */

#pragma once

#include "config.h"

#include "object.h"
#include "common.h"

#include <string>
#include <map>

#include <SDL/SDL_ttf.h>


struct FontOptions
{
  FontOptions()
    { fileName = ""; pointSize = 0; }
  FontOptions(const std::string &pFileName, int pPointSize)
    { fileName = pFileName; pointSize = pPointSize; }
  FontOptions(const FontOptions &options)
    { fileName = options.fileName; pointSize = options.pointSize; }

  std::string fileName;
  int pointSize;

  inline bool operator==(const FontOptions &o) const
    { return (fileName == o.fileName) && (pointSize == o.pointSize); }

  inline bool operator<(const FontOptions &o) const
    { return (fileName < o.fileName) ||
             ((fileName == o.fileName) && (pointSize < o.pointSize)); }
};

class Font;
class WindowResizeEvent;

class FontManager : public Object
{
  public:
    FontManager();
    virtual ~FontManager();

    inline static FontManager* instance()
      { return _instance; }

    Font* getFont(const std::string &fontPath, int pointSize);
    Font* getFont(const FontOptions &options);

    void windowResized(WindowResizeEvent *event);

  private:
    static FontManager *_instance;
    typedef std::map<FontOptions, Font*> FontMap;
    typedef std::map<FontOptions, Font*>::iterator FontMapIterator;
    FontMap _fontMap;
    Size _referenceWindowSize;
};

class FontMetrics;

class Font : public Object
{
  private:
    Font() : Object(""), _font(NULL) { }

  public:
    explicit Font(const FontOptions &pOptions);
    virtual ~Font();

    inline const bool valid() const
      { return _font != NULL; }

    inline const FontOptions options() const
      { return _fontOptions; }

    void renderText(const std::string &text, const Point &location);

    void renderTextUTF8(const std::string &text, const Point &location);

    int cacheStaticText(const std::string &text, bool utf8 = false);
    Size staticTextMetrics(int id);

    void renderStaticText(int id, const Point &location);

    void clearCache();

    void changeOptions(const FontOptions &newOptions);

  private:
    struct CachedTexture
    {
      CachedTexture()
        { id = width = height = charWidth = charHeight = 0; }

      unsigned int id;
      short int width, height, charWidth, charHeight;

      inline int operator==(const CachedTexture &other) const
        { return id == other.id; }
    };

    struct UTF8Char
    {
      UTF8Char()
        { reset(); }

      inline void reset()
        { bytes[0] = bytes[1] = bytes[2] = bytes[3] = bytes[4] = '\0'; }

      char bytes[5];

      bool operator==(const UTF8Char &other) const
      {
        for (int i = 0; i < 5; ++i)
        {
          if (bytes[i] != other.bytes[i])
            return false;
        }

        return true;
      }

      bool operator<(const UTF8Char &other) const
      {
        for (int i = 0; i < 5; ++i)
        {
          if (bytes[i] < other.bytes[i])
            return true;
          else if (bytes[i] > other.bytes[i])
            return false;
        }
        return false;
      }
    };

    template<class Key>
    class TextureCache
    {
      public:
        void add(Key k, const CachedTexture &v)
        {
          _cache[k] = v;
        }

        CachedTexture search(const Key &k) const
        {
          typename std::map<Key, CachedTexture>::const_iterator
             it = _cache.find(k);
          if (it != _cache.end())
            return (*it).second;

          return CachedTexture();
        }

        void clear()
        {
          _cache.clear();
        }

      private:
        std::map<Key, CachedTexture> _cache;
    };

  private:
    FontOptions _fontOptions;
    TTF_Font *_font;
    TextureCache<char> _asciiCache;
    TextureCache<UTF8Char> _utf8Cache;
    TextureCache<int> _staticCache;
    static int _staticTextId;

    CachedTexture createTexture(const std::string &text, bool utf8);

    static int nextPowerOfTwo(int x);

  friend class FontMetrics;
};

class FontMetrics
{
  private:
    FontMetrics() : _font(NULL) { }

  public:
    explicit FontMetrics(Font *pFont) : _font(pFont) { }
    virtual ~FontMetrics() { _font = NULL; }

    int height();

    int width(const std::string &text);
    int widthUTF8(const std::string &text);

    Size textSize(const std::string &text);
    Size textSizeUTF8(const std::string &text);

  private:
    Font *_font;
};
