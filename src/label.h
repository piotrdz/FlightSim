/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* label.h
    Zawiera klasę Label, która pozwala na wyświetlenie napisu o danym
    kolorze, ustawieniu itp. */

#pragma once

#include "config.h"

#include "common.h"
#include "widget.h"

#include <string>

class Font;

class Label : public Widget
{
  public:
    Label(Widget *pParent,
          const std::string &pText = "",
          Font *pFont = NULL,
          char pAlignment = AL_Center,
          bool pUTF8Flag = false,
          const Color &pColor = Color(1.0f, 1.0f, 1.0f),
          bool pStaticFlag = false,
          const std::string &name = "");
    virtual ~Label();

    inline std::string text() const
      { return _text; }
    void setText(const std::string &pText);

    inline Font* font() const
      { return _font; }
    void setFont(Font *pFont);

    inline char alignment()
      { return _alignment; }
    void setAlignment(char pAlignment);

    inline bool utf8Flag() const
      { return _utf8Flag; }
    void setUTF8Flag(bool pUTF8Flag);

    inline Color color() const
      { return _color; }
    inline void setColor(const Color &pColor)
      { _color = pColor; }

    inline bool staticFlag() const
      { return _staticFlag; }
    void setStaticFlag(bool pStaticFlag);

    inline Rect textGeometry() const
      { return _textGeometry; }

    virtual Size preferredSize() const
      { return _textGeometry.size(); }

    virtual void init();
    virtual void render();

  private:
    std::string _text;
    Font *_font;
    char _alignment;
    bool _utf8Flag, _staticFlag;
    Color _color;
    int _staticID;
    Rect _textGeometry;

    void updateTextGeometry();
    void updateStaticID();

  protected:
    virtual void fontResizeEvent(FontResizeEvent *);
    virtual void resizeEvent();
};
