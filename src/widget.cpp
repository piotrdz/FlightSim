/***************************************************************************
 *   Copyright (C) 2011-2012 by Piotr Dziwinski                            *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* widget.cpp
    Contains the implementation of the Widget class. */

#include "widget.h"

#include <algorithm>
#include <cassert>

using namespace std;


bool WidgetComparator::operator()(Widget* left, Widget* right)
{
  return ((left->_renderPriority > right->_renderPriority) ||
          ((left->_renderPriority == right->_renderPriority) && (left < right)));
}


Widget::Widget(Widget *pParent, const string &pName, int pRenderPriority)
  : Object(pName.empty() ? genericName("Widget") : pName)
{
  _parent = pParent;
  _renderPriority = pRenderPriority;

  _parentDeletion = false;

  _visible = false;
  _eventMask = ET_WindowResize | ET_FontResize;

  if (_parent != NULL)
    _parent->addChild(this);
}

Widget::~Widget()
{
  for (WidgetChildrenIterator it = _children.begin();
       it != _children.end(); ++it)
  {
    (*it)->_parentDeletion = true;
    delete *it;
  }
  _children.clear();

  if ((_parent != NULL) && (!_parentDeletion))
    _parent->removeChild(this);

  _parent = NULL;
}

void Widget::setPosition(const Point &pPosition)
{
  _geometry = Rect(pPosition, _geometry.size());
  resizeEvent();
}

void Widget::setSize(const Size &pSize)
{
  _geometry = Rect(_geometry.position(), pSize);
  resizeEvent();
}

void Widget::setGeometry(const Rect &pGeometry)
{
  _geometry = pGeometry;
  resizeEvent();
}

void Widget::setRenderPriority(int newPriority)
{
  int oldPriority = _renderPriority;
  _renderPriority = newPriority;

  if ((_parent != NULL) && (_renderPriority != oldPriority))
    _parent->reorderChildren();
}

void Widget::setVisible(bool pVisible)
{
  if (_visible != pVisible)
  {
    _visible = pVisible;

    if (_visible)
      showEvent();
    else
      hideEvent();
  }
}

void Widget::changeParent(Widget *newParent)
{
  if (_parent != NULL)
    _parent->removeChild(this);

  _parent = newParent;
  if (_parent != NULL)
    _parent->addChild(this);
}

void Widget::addChild(Widget *child)
{
  _children.insert(child);
}

bool Widget::removeChild(Widget *child)
{
  WidgetChildrenIterator it = find(_children.begin(), _children.end(), child);
  if (it == _children.end())
    return false;

  _children.erase(it);

  return true;
}

Size Widget::preferredSize() const
{
  return Size();
}

void Widget::initChildren()
{
  for (WidgetChildrenIterator it = _children.begin();
       it != _children.end(); ++it)
  {
    (*it)->init();
    (*it)->initChildren();
  }
}

void Widget::renderChildren()
{
  for (WidgetChildrenIterator it = _children.begin();
       it != _children.end(); ++it)
  {
    if ((*it)->visible())
    {
      (*it)->render();
      (*it)->renderChildren();
    }
  }
}

void Widget::updateChildren()
{
  for (WidgetChildrenIterator it = _children.begin();
       it != _children.end(); ++it)
  {
    if ((*it)->visible())
    {
      (*it)->update();
      (*it)->updateChildren();
    }
  }
}

void Widget::sendEvent(Event *event)
{
  if ((event->type() & _eventMask) == 0)
    return;

  if (((event->type() & ET_InputEvents) != 0) && (!_visible))
    return;

  switch (event->type())
  {
    case ET_WindowResize:
      windowResizeEvent((WindowResizeEvent*)event);
      break;

    case ET_FontResize:
      fontResizeEvent((FontResizeEvent*)event);
      break;

    case ET_KeyboardDown:
      keyboardDownEvent((KeyboardDownEvent*)event);
      break;

    case ET_KeyboardUp:
      keyboardUpEvent((KeyboardUpEvent*)event);
      break;

    case ET_MouseButtonDown:
      mouseButtonDownEvent((MouseButtonDownEvent*)event);
      break;

    case ET_MouseButtonUp:
      mouseButtonUpEvent((MouseButtonUpEvent*)event);
      break;

    case ET_MouseMotion:
      mouseMotionEvent((MouseMotionEvent*)event);
      break;

    case ET_JoystickAxisMotion:
      joystickAxisMotionEvent((JoystickAxisMotionEvent*)event);
      break;

    case ET_JoystickButtonDown:
      joystickButtonDownEvent((JoystickButtonDownEvent*)event);
      break;

    case ET_JoystickButtonUp:
      joystickButtonUpEvent((JoystickButtonUpEvent*)event);
      break;

    case ET_JoystickHat:
      joystickHatEvent((JoystickHatEvent*)event);
      break;

    default:
      return;
  }

  if (event->pass())
  {
    for (WidgetChildrenIterator it = _children.begin();
         it != _children.end(); ++it)
      (*it)->sendEvent(event);
  }
}

void Widget::sendChildEvent(int parameter)
{
  if (_parent == NULL)
    print("sendChildEvent(): NULL parent!");
  else
    _parent->childEvent(this, parameter);
}

void Widget::reorderChildren()
{
  WidgetChildren newChildren;
  for (WidgetChildrenIterator it = _children.begin();
       it != _children.end(); ++it)
  {
    newChildren.insert(*it);
  }
}
