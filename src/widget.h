/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* widget.h
    Contains the Widget class - base class for all rendered objects. */

#pragma once

#include "config.h"

#include "common.h"
#include "object.h"
#include "events.h"

#include <set>

class Widget;

class WidgetComparator
{
  public:
    bool operator()(Widget *left, Widget *right);
};

class Widget : public Object
{
  friend class WidgetComparator;

  public:
    typedef std::set<Widget*, WidgetComparator> WidgetChildren;
    typedef std::set<Widget*, WidgetComparator>::iterator
            WidgetChildrenIterator;
    typedef std::set<Widget*, WidgetComparator>::const_iterator
            WidgetChildrenConstIterator;

  public:
    Widget(Widget *pParent, const std::string &pName,
           int pRenderPriority = 0);
    virtual ~Widget();

    inline Point position() const
      { return _geometry.position(); }
    void setPosition(const Point &pPosition);

    inline Size size() const
      { return _geometry.size(); }
    void setSize(const Size &pSize);

    inline Rect geometry() const
      { return _geometry; }
    void setGeometry(const Rect &pGeometry);

    inline bool visible() const
      { return _visible; }
    void setVisible(bool pRenderFlag);

    inline void show()
      { setVisible(true); }
    inline void hide()
      { setVisible(false); }

    inline int eventMask() const
      { return _eventMask; }
    inline void setEventMask(int pEventMask)
      { _eventMask = pEventMask; }

    inline void enableEvents(int mask)
      { _eventMask |= mask; }
    inline void disableEvents(int mask)
      { _eventMask &= ~mask; }

    inline void enableInput()
      { enableEvents(ET_InputEvents); }
    inline void disableInput()
      { disableEvents(ET_InputEvents); }

    inline int renderPriority() const
      { return _renderPriority; }
    void setRenderPriority(int newPriority);

    inline Widget* parent()
      { return _parent; }

    void changeParent(Widget *newParent);

    inline WidgetChildren* children()
      { return &_children;}

    void addChild(Widget *child);
    bool removeChild(Widget *child);


    virtual Size preferredSize() const;


    virtual void init() {}
    virtual void initChildren();

    virtual void render() {}
    virtual void renderChildren();

    virtual void update() {}
    virtual void updateChildren();


    virtual void sendEvent(Event *);

  protected:
    virtual void windowResizeEvent(WindowResizeEvent *e) {}
    virtual void fontResizeEvent(FontResizeEvent *e) {}

    virtual void keyboardDownEvent(KeyboardDownEvent *e) {}
    virtual void keyboardUpEvent(KeyboardUpEvent *e) {}

    virtual void mouseButtonDownEvent(MouseButtonDownEvent *e) { }
    virtual void mouseButtonUpEvent(MouseButtonUpEvent *e) {}
    virtual void mouseMotionEvent(MouseMotionEvent *e) {}

    virtual void joystickAxisMotionEvent(JoystickAxisMotionEvent *e) {}
    virtual void joystickButtonDownEvent(JoystickButtonDownEvent *e) {}
    virtual void joystickButtonUpEvent(JoystickButtonUpEvent *e) {}
    virtual void joystickHatEvent(JoystickHatEvent *e) {}


    virtual void resizeEvent() {}
    virtual void showEvent() {}
    virtual void hideEvent() {}

    virtual void childEvent(Widget *sender, int parameter) {}

    virtual void sendChildEvent(int type);

  private:
    Widget *_parent;
    WidgetChildren _children;
    Rect _geometry;
    bool _visible;
    int _eventMask;
    int _renderPriority;
    bool _parentDeletion;

    void reorderChildren();
};
