/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

  /* controls.cpp
     Zawiera implementację klas Control, Button, CheckBox, ChoiceBox
     i LineEdit. */

#include "controls.h"

#include "decorator.h"
#include "fontengine.h"

using namespace std;


Control::Control(Widget *pParent, const string &pName)
  : Widget(pParent, pName.empty() ? genericName("Control") : pName)
{
  enableInput();
}

Control::~Control()
{
}

void Control::setFocus(int pFocus)
{
  _focus = pFocus;
  if (pFocus == 0)
  {
    disableEvents(ET_InputEvents);
    focusOut();
  }
  else
  {
    enableEvents(ET_InputEvents);
    focusIn();
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Button::Button(Widget *pParent,
               const string &pText,
               bool pUTF8Flag,
               Font *pCustomFont,
               const string &pName)
  : Control(pParent, pName.empty() ? genericName("Button") : pName)
{
  Font *f = (pCustomFont != NULL) ? pCustomFont :
               Decorator::instance()->getFont(FT_Button);
  _label = new Label(this, pText, f,
                     AL_Center, pUTF8Flag,
                     Decorator::instance()->getColor(C_Text));
  _label->show();
}

Button::~Button()
{
  _label = NULL;
}

Size Button::preferredSize() const
{
  Size result;
  result.w = 4.0f * Decorator::instance()->getDefaultMargin() +
             _label->textGeometry().w;
  result.h = _label->textGeometry().h * 1.8f;
  return result;
}

void Button::render()
{
  Decorator::instance()->renderButtonFrame(geometry(), _focus);
}

void Button::focusIn()
{
  if (_focus == 1)
    _label->setColor(Decorator::instance()->getColor(C_HalfFocusedText));
  else
    _label->setColor(Decorator::instance()->getColor(C_FocusedText));
}

void Button::focusOut()
{
  _label->setColor(Decorator::instance()->getColor(C_Text));
}

void Button::resizeEvent()
{
  _label->setGeometry(geometry());
}

void Button::keyboardDownEvent(KeyboardDownEvent *e)
{
  if ((e->event().keysym.sym == SDLK_RETURN) ||
      (e->event().keysym.sym == SDLK_KP_ENTER))
    sendChildEvent(Clicked);
}

void Button::mouseButtonUpEvent(MouseButtonUpEvent *e)
{
  Point mousePos(e->event().x, e->event().y);
  if (geometry().pointInside(mousePos))
    sendChildEvent(Clicked);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CheckBox::CheckBox(Widget *pParent,
                   const string &pText,
                   bool pChecked,
                   bool pUTF8Flag,
                   Font *pCustomFont,
                   const string &pName)
  : Control(pParent, pName.empty() ? genericName("CheckBox") : pName)
{
  Font *f = (pCustomFont != NULL) ? pCustomFont :
               Decorator::instance()->getFont(FT_Normal);
  _label = new Label(this, pText, f,
                     AL_Left | AL_VCenter, pUTF8Flag,
                     Decorator::instance()->getColor(C_Text));
  _label->show();
  _checked = pChecked;
}

CheckBox::~CheckBox()
{
  _label = NULL;
}

Size CheckBox::preferredSize() const
{
  Size result;
  result.w = Decorator::instance()->getDefaultMargin() +
             _label->textGeometry().h + _label->textGeometry().w;
  result.h = _label->textGeometry().h;
  return result;
}

void CheckBox::render()
{
  Decorator::instance()->renderCheckBoxFrame(_boxFrame, _checked, _focus);
}

void CheckBox::focusIn()
{
  if (_focus == 1)
    _label->setColor(Decorator::instance()->getColor(C_HalfFocusedText));
  else
    _label->setColor(Decorator::instance()->getColor(C_FocusedText));
}

void CheckBox::focusOut()
{
  _label->setColor(Decorator::instance()->getColor(C_Text));
}

void CheckBox::resizeEvent()
{
  float textHeight = _label->textGeometry().h;
  _boxFrame.x = geometry().x;
  _boxFrame.y = geometry().y + 0.5f * (geometry().h - textHeight);
  _boxFrame.w = textHeight;
  _boxFrame.h = textHeight;

  Rect labelGeometry;
  labelGeometry.x = _boxFrame.x2() + Decorator::instance()->getDefaultMargin();
  labelGeometry.y = geometry().y;
  labelGeometry.w = geometry().x2() - labelGeometry.x;
  labelGeometry.h = geometry().h;
  _label->setGeometry(labelGeometry);
}

void CheckBox::keyboardDownEvent(KeyboardDownEvent *e)
{
  if ((e->event().keysym.sym == SDLK_RETURN) ||
      (e->event().keysym.sym == SDLK_KP_ENTER) ||
      (e->event().keysym.sym == SDLK_SPACE))
  {
    _checked = !_checked;
    sendChildEvent(_checked ? Checked : Unchecked);
  }
}

void CheckBox::mouseButtonUpEvent(MouseButtonUpEvent *e)
{
  Point mousePos(e->event().x, e->event().y);
  if (geometry().pointInside(mousePos))
  {
    _checked = !_checked;
    sendChildEvent(_checked ? Checked : Unchecked);
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

ChoiceBox::ChoiceBox(Widget *pParent,
                     const vector<string> &pChoices,
                     unsigned int pIndex,
                     bool pUTF8Flag,
                     Font *pCustomFont,
                     const string &pName)
  : Control(pParent, pName.empty() ? genericName("ChoiceBox") : pName)
{
  Font *f = (pCustomFont != NULL) ? pCustomFont :
               Decorator::instance()->getFont(FT_Normal);
  _label = new Label(this, "", f,
                     AL_Center, pUTF8Flag,
                     Decorator::instance()->getColor(C_Text));
  _label->show();

  _choices = pChoices;
  _focusedBox = 0;

  setIndex(pIndex);

  updatePreferredSize();
}

ChoiceBox::~ChoiceBox()
{
  _label = NULL;
}

void ChoiceBox::setChoices(std::vector<std::string> &pChoices)
{
  _choices = pChoices;
  _index = 0;
  updatePreferredSize();
}

void ChoiceBox::setIndex(unsigned int pIndex)
{
  if (_choices.empty())
  {
    _index = 0;
    _label->setText("");
    return;
  }

  _index = pIndex % _choices.size();
  _label->setText(_choices[_index]);
}

void ChoiceBox::updatePreferredSize()
{
  float maxWidth = 0.0f;
  FontMetrics metrics(_label->font());
  for (unsigned int i = 0; i < _choices.size(); ++i)
  {
    float w = 0;
    if (_label->utf8Flag())
      w = metrics.widthUTF8(_choices[i]);
    else
      w = metrics.width(_choices[i]);
    if (w > maxWidth)
      maxWidth = w;
  }
  _preferredSize.w = 4.0f * Decorator::instance()->getDefaultMargin() +
                     + 2.0f * metrics.height() + maxWidth;
  _preferredSize.h = 1.5f * metrics.height();
}

void ChoiceBox::render()
{
  Decorator::instance()->renderChoiceBoxFrame(geometry(), _focusedBox, _focus);
}

void ChoiceBox::focusIn()
{
  if (_focus == 1)
    _label->setColor(Decorator::instance()->getColor(C_HalfFocusedText));
  else
    _label->setColor(Decorator::instance()->getColor(C_FocusedText));
}

void ChoiceBox::focusOut()
{
  _label->setColor(Decorator::instance()->getColor(C_Text));
}

void ChoiceBox::resizeEvent()
{
  Rect labelGeometry = geometry();
  labelGeometry.w -= 2.0f * labelGeometry.h;
  _label->setGeometry(labelGeometry);

  _box1Rect.x = labelGeometry.x2();
  _box1Rect.y = geometry().y;
  _box1Rect.w = geometry().h;
  _box1Rect.h = geometry().h;

  _box2Rect.x = _box1Rect.x2();
  _box2Rect.y = geometry().y;
  _box2Rect.w = geometry().h;
  _box2Rect.h = geometry().h;
}

void ChoiceBox::keyboardDownEvent(KeyboardDownEvent *e)
{
  if (e->event().keysym.sym == SDLK_LEFT)
  {
    unsigned int newIndex = 0;
    if (newIndex == 0)
      newIndex = _choices.size() - 1;
    else
      newIndex = _index - 1;

    setIndex(newIndex);
    sendChildEvent(ChoiceChanged);
  }
  else if (e->event().keysym.sym == SDLK_RIGHT)
  {
    unsigned int newIndex = _index + 1;
    if (newIndex >= _choices.size())
      newIndex = 0;

    setIndex(newIndex);
    sendChildEvent(ChoiceChanged);
  }
}

void ChoiceBox::mouseMotionEvent(MouseMotionEvent *e)
{
  Point mousePos(e->event().x, e->event().y);
  if (_box1Rect.pointInside(mousePos))
    _focusedBox = 1;
  else if (_box2Rect.pointInside(mousePos))
    _focusedBox = 2;
  else
    _focusedBox = 0;
}

void ChoiceBox::mouseButtonUpEvent(MouseButtonUpEvent *e)
{
  Point mousePos(e->event().x, e->event().y);
  if (_box1Rect.pointInside(mousePos))
  {
    unsigned int newIndex = 0;
    if (_index == 0)
      newIndex = _choices.size() - 1;
    else
      newIndex = _index - 1;

    setIndex(newIndex);
    sendChildEvent(ChoiceChanged);
  }
  else if (_box2Rect.pointInside(mousePos))
  {
    unsigned int newIndex = _index + 1;
    if (newIndex >= _choices.size())
      newIndex = 0;

    setIndex(newIndex);
    sendChildEvent(ChoiceChanged);
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

LineEdit::LineEdit(Widget *pParent,
                   const string &pText,
                   bool pUTF8Flag,
                   Font *pCustomFont,
                   const string &pName)
  : Control(pParent, pName.empty() ? genericName("LineEdit") : pName)
{
  Font *f = (pCustomFont != NULL) ? pCustomFont :
               Decorator::instance()->getFont(FT_Normal);
  _label = new Label(this, "", f,
                     AL_Left | AL_VCenter, pUTF8Flag,
                     Decorator::instance()->getColor(C_Text));
  _label->show();
  _blinkTimer.setIntervalMsec(1000);
  _blinkTimer.setEnabled(false);
  _cursorVisible = false;
  _indicator = 0;
  setText(pText);
}

LineEdit::~LineEdit()
{
  _label = NULL;
}

string LineEdit::text() const
{
  return unicodeStringToUtf8(_text);
}

void LineEdit::setText(const string &pText)
{
  _text = _previousText = utf8StringToUnicode(pText);
  _textOffset = 0;
  _cursorPos = 0;
  updateLabel();
}

void LineEdit::updateLabel()
{
  _indicator = 0;

  FontMetrics metrics(_label->font());
  _cursorX = _label->geometry().x;
  _cursorHeight = metrics.height();

  if (_cursorPos < _textOffset)
    _textOffset = _cursorPos;

  if ((geometry().w == 0) || (geometry().h == 0))
    return;

  if (_text.empty())
  {
    _label->setText("");
    return;
  }

  string sub = unicodeStringToUtf8(_text.substr(_textOffset,
                                                _cursorPos - _textOffset));
  if (utf8Flag())
    _cursorX = _label->geometry().x + metrics.widthUTF8(sub);
  else
    _cursorX = _label->geometry().x + metrics.width(sub);

  if (_cursorX < _label->geometry().x2())
  {
    if (_textOffset > 0)
      _indicator = 1;

    string labelText = sub;
    unsigned int i = _cursorPos;
    while (i < _text.size())
    {
      string newLabelText = labelText + unicodeCharToUtf8(_text[i]);

      float w = 0;
      if (utf8Flag())
        w = metrics.widthUTF8(newLabelText);
      else
        w = metrics.width(newLabelText);

      if (w >= _label->geometry().w)
      {
        _indicator = 2;
        break;
      }

      labelText = newLabelText;
      ++i;
    }
    _label->setText(labelText);
  }
  else
  {
    _indicator = 1;
    string labelText = sub;
    unsigned int newOffset = _cursorPos - 1;
    while (newOffset > 0)
    {
      string newLabelText = unicodeStringToUtf8(
                              _text.substr(newOffset, _cursorPos - newOffset));

      float w = 0;
      if (utf8Flag())
        w = metrics.widthUTF8(newLabelText);
      else
        w = metrics.width(newLabelText);

      if (w >= _label->geometry().w)
      {
        ++newOffset;
        break;
      }

      labelText = newLabelText;
      --newOffset;
    }
    _textOffset = newOffset;
    _label->setText(labelText);

    sub = unicodeStringToUtf8(
            _text.substr(_textOffset, _cursorPos - _textOffset));
    if (utf8Flag())
      _cursorX = _label->geometry().x + metrics.widthUTF8(sub);
    else
      _cursorX = _label->geometry().x + metrics.width(sub);
  }
}

Size LineEdit::preferredSize() const
{
  FontMetrics metrics(_label->font());
  Size prefSize;
  if (utf8Flag())
    prefSize = metrics.textSizeUTF8(text());
  else
    prefSize = metrics.textSize(text());

  prefSize.w += 2.0f * Decorator::instance()->getDefaultMargin();
  prefSize.h *= 1.5f;

  return prefSize;
}

void LineEdit::focusIn()
{
  if (_focus == 1)
    _label->setColor(Decorator::instance()->getColor(C_HalfFocusedText));
  else
    _label->setColor(Decorator::instance()->getColor(C_FocusedText));

  _blinkTimer.setEnabled(true);
}

void LineEdit::focusOut()
{
  _label->setColor(Decorator::instance()->getColor(C_Text));
  _blinkTimer.setEnabled(false);
  _cursorVisible = false;
  if (_text != _previousText)
    sendChildEvent(TextChangedOnFocusOut);
}

void LineEdit::render()
{
  Decorator::instance()->renderEditFrame(geometry(), _cursorVisible,
                                         _cursorX, _cursorHeight,
                                         _indicator, _focus);
}

void LineEdit::update()
{
  if (_blinkTimer.checkTimeout())
    _cursorVisible = !_cursorVisible;
}

void LineEdit::resizeEvent()
{
  Rect labelGeometry = geometry();
  labelGeometry.x += Decorator::instance()->getDefaultMargin();
  labelGeometry.w -= 2.0f * Decorator::instance()->getDefaultMargin();
  _label->setGeometry(labelGeometry);
  updateLabel();
}

void LineEdit::keyboardDownEvent(KeyboardDownEvent *e)
{
  int keysym = e->event().keysym.sym;
  if (keysym == SDLK_LEFT)
  {
    if (_cursorPos > 0)
    {
      --_cursorPos;
      updateLabel();
    }
  }
  else if (keysym == SDLK_RIGHT)
  {
    if (_cursorPos < _text.size())
    {
      ++_cursorPos;
      updateLabel();
    }
  }
  else if (keysym == SDLK_HOME)
  {
    _cursorPos = 0;
    updateLabel();
  }
  else if (keysym == SDLK_END)
  {
    _cursorPos = _text.size();
    updateLabel();
  }
  else if (keysym == SDLK_DELETE)
  {
    if (_cursorPos < _text.size())
    {
      _previousText = _text;

      wstring s1, s2;
      if (_cursorPos != 0)
        s1 = _text.substr(0, _cursorPos);
      if (_cursorPos != _text.size() - 1)
        s2 = _text.substr(_cursorPos + 1);

      _text = s1 + s2;
      updateLabel();

      sendChildEvent(TextChanged);
    }
  }
  else if (keysym == SDLK_BACKSPACE)
  {
    if (_cursorPos > 0)
    {
      _previousText = _text;

      wstring s1, s2;
      if (_cursorPos != 1)
        s1 = _text.substr(0, _cursorPos - 1);
      if (_cursorPos != _text.size())
        s2 = _text.substr(_cursorPos);

      _text = s1 + s2;
      --_cursorPos;
      updateLabel();

      sendChildEvent(TextChanged);
    }
  }
  else if ((keysym == SDLK_RETURN) || (keysym == SDLK_KP_ENTER))
  {
    _previousText = _text;
    sendChildEvent(ReturnPressed);
  }
  else if (e->event().keysym.unicode != 0)
  {
    _previousText = _text;

    wchar_t ch = (wchar_t)(e->event().keysym.unicode);
    _text.insert(_cursorPos, 1, ch);
    ++_cursorPos;
    updateLabel();

    sendChildEvent(TextChanged);
  }
}

void LineEdit::mouseButtonUpEvent(MouseButtonUpEvent *e)
{
  Point mousePos(e->event().x, e->event().y);
  if (geometry().pointInside(mousePos))
  {
    if (mousePos.x < _label->geometry().x)
    {
      _cursorPos = _textOffset;
    }
    else if (mousePos.x > _label->geometry().x2())
    {
      _cursorPos = _textOffset + _label->text().size();
    }
    else
    {
      FontMetrics metrics(_label->font());
      string sub;
      bool found = false;
      for (unsigned int i = _textOffset; i < _text.size(); ++i)
      {
        sub += unicodeCharToUtf8(_text[i]);
        float width = 0.0f;
        if (utf8Flag())
          width = metrics.widthUTF8(sub);
        else
          width = metrics.width(sub);

        if (width > mousePos.x - _label->geometry().x)
        {
          _cursorPos = i;
          found = true;
          break;
        }
      }
      if (!found)
        _cursorPos = _textOffset + _label->text().size();
    }

    _blinkTimer.reset();
    _cursorVisible = true;
    updateLabel();
  }
}
