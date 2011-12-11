/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* label.cpp
    Zawiera implementację klasy Label. */

#include "label.h"

#include "fontengine.h"

#include <GL/gl.h>

using namespace std;


Label::Label(Widget *pParent,
             const string &pText,
             Font *pFont,
             char pAlignment,
             bool pUTF8Flag,
             const Color &pColor,
             bool pStaticFlag,
             const string &name)
  : Widget(pParent, name.empty() ? genericName("Label") : name)
{
  _staticID = -1;
  _text = pText;
  _font = pFont;
  _alignment = pAlignment;
  _utf8Flag = pUTF8Flag;
  _color = pColor;
  _staticFlag = pStaticFlag;
}

Label::~Label()
{
  _font = NULL;
}

void Label::init()
{
  if (_font == NULL)
    return;

  if (_staticFlag)
    updateStaticID();

  Size textSize;
  if (_staticFlag)
    textSize = _font->staticTextMetrics(_staticID);
  else
  {
    FontMetrics metrics(_font);
    if (_utf8Flag)
      textSize = metrics.textSizeUTF8(_text);
    else
      textSize = metrics.textSize(_text);
  }

  _textGeometry.w = textSize.w;
  _textGeometry.h = textSize.h;
}

void Label::setText(const string &pText)
{
  _text = pText;

  if (_staticFlag)
    updateStaticID();

  updateTextGeometry();
}

void Label::setFont(Font *pFont)
{
  _font = pFont;

  if (_staticFlag)
    updateStaticID();

  updateTextGeometry();
}

void Label::setAlignment(char pAlignment)
{
  _alignment = pAlignment;
  updateTextGeometry();
}

void Label::setUTF8Flag(bool pUTF8Flag)
{
  bool before = _utf8Flag;
  _utf8Flag = pUTF8Flag;

  if ((_staticFlag) && (before != _utf8Flag))
    updateStaticID();
}

void Label::setStaticFlag(bool pStaticFlag)
{
  bool before = _staticFlag;
  _staticFlag = pStaticFlag;

  if ((_staticFlag) && (!before))
    updateStaticID();
}

void Label::resizeEvent()
{
  updateTextGeometry();
}

void Label::fontResizeEvent(FontResizeEvent *)
{
  if (_staticFlag)
    updateStaticID();
}

void Label::updateTextGeometry()
{
  if (_font == NULL)
    return;

  Size textSize;
  if (_staticFlag)
    textSize = _font->staticTextMetrics(_staticID);
  else
  {
    FontMetrics metrics(_font);
    if (_utf8Flag)
      textSize = metrics.textSizeUTF8(_text);
    else
      textSize = metrics.textSize(_text);
  }

  _textGeometry.w = textSize.w;
  _textGeometry.h = textSize.h;

  if ((_alignment & AL_Left) && (_alignment & AL_Right))
    _textGeometry.x = (size().w - textSize.w) / 2.0;
  else if (_alignment & AL_Left)
    _textGeometry.x = 0;
  else if (_alignment & AL_Right)
    _textGeometry.x = size().w - textSize.w;
  else
    _textGeometry.x = (size().w - textSize.w) / 2.0;

  if ((_alignment & AL_Top) && (_alignment & AL_Bottom))
    _textGeometry.y = (size().h - textSize.h) / 2.0;
  else if (_alignment & AL_Top)
    _textGeometry.y = 0;
  else if (_alignment & AL_Bottom)
    _textGeometry.y = size().h - textSize.h;
  else
    _textGeometry.y = (size().h - textSize.h) / 2.0;
}

void Label::updateStaticID()
{
  if ((_font != NULL) && (!_text.empty()))
    _staticID = _font->cacheStaticText(_text, _utf8Flag);
}

void Label::render()
{
  if ((_font == NULL) || _text.empty())
    return;

  glPushMatrix();
  {
    glTranslatef(position().x, position().y, 0);

    glColor4fv(_color);

    if (_color.a < 1.0f)
    {
      glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }

    if (_staticFlag)
      _font->renderStaticText(_staticID, _textGeometry);
    else
    {
      if (_utf8Flag)
        _font->renderTextUTF8(_text, _textGeometry);
      else
        _font->renderText(_text, _textGeometry);
    }

    if (_color.a < 1.0f)
    {
      glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_DECAL);
    }
  }
  glPopMatrix();
}
