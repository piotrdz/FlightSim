/***************************************************************************
 *   Copyright (C) 2011-2012 by Piotr Dziwinski                            *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* dialog.cpp
    Contains the implementation of the Dialog class. */

#include "dialog.h"

#include "decorator.h"
#include "render.h"

#include <sstream>

using namespace std;


Dialog::Dialog(Widget *pParent,
               const std::string &pName)
  : Widget(pParent, pName.empty() ? genericName("Dialog") : pName)
{
  _focusing = true;
  _focusIndex = 0;

  _titleLabel = NULL;
}

Dialog::~Dialog()
{
  _titleLabel = NULL;
}

void Dialog::init()
{
  _titleLabel = new Label(this, "",
                          Decorator::instance()->getFont(FT_WindowTitle),
                          AL_Center, false,
                          Decorator::instance()->getColor(C_Text));
  _titleLabel->show();
}

void Dialog::keyboardDownEvent(KeyboardDownEvent *e)
{
  if ((!_focusing) || _focusControls.empty())
    return;

  if (e->event().keysym.sym == SDLK_TAB)
  {
    unsigned int newIndex = _focusIndex;

    if (e->event().keysym.mod & KMOD_SHIFT)
    {
      do
      {
        if (newIndex == 0)
          newIndex = _focusControls.size() - 1;
        else
          --newIndex;
      } while (_focusControls[newIndex]->visible() == false);
    }
    else
    {
      do
      {
        ++newIndex;
        if (newIndex >= _focusControls.size())
          newIndex = 0;
      } while (_focusControls[newIndex]->visible() == false);
    }

    focusControl(_focusControls[newIndex], 1);

    e->stop();
  }
}

void Dialog::mouseMotionEvent(MouseMotionEvent *e)
{
  if ((!_focusing) || _focusControls.empty())
    return;

  Point mousePos(e->event().x, e->event().y);

  bool found = false;
  for (unsigned int i = 0; i < _focusControls.size(); ++i)
  {
    if (_focusControls[i]->geometry().pointInside(mousePos))
    {
      focusControl(_focusControls[i], 2);
      found = true;
      break;
    }
  }

  if (!found)
    focusControl(_focusControls[_focusIndex], 1);
}

void Dialog::resizeEvent()
{
  float titleHeight = _titleLabel->textGeometry().h;

  Rect titleRect = geometry();
  titleRect.h = titleHeight;
  _titleLabel->setGeometry(titleRect);

  float margin = Decorator::instance()->getDialogMargin();
  _clientGeometry.x = geometry().x + margin;
  _clientGeometry.y = geometry().y + titleHeight + margin;
  _clientGeometry.w = geometry().w - 2.0f * margin;
  _clientGeometry.h = geometry().h - titleHeight - 2.0f * margin;
}

void Dialog::render()
{
  Decorator::instance()->renderWindowFrame(geometry(),
                                           _titleLabel->textGeometry().h);
}

void Dialog::focusControl(Control *control, int focus)
{
  for (unsigned int i = 0; i < _focusControls.size(); ++i)
  {
    if (_focusControls[i] != control)
    {
      _focusControls[i]->setFocus(0);
    }
    else
    {
      _focusIndex = i;
      _focusControls[i]->setFocus(focus);
    }
  }
}

void Dialog::addFocusControl(Control *control)
{
  _focusControls.push_back(control);
  focusControl(_focusControls[_focusIndex], 1);
}
