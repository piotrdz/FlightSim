/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* events.h
    Zawiera klasy zdarzeń, które są generowane w wyniku działania
    użytkownika lub zmiany rozmiaru okna. */

#pragma once

#include "config.h"

#include <SDL/SDL_events.h>

enum EventType
{
  ET_WindowResize = 0x001,
  ET_FontResize = 0x002,

  ET_KeyboardDown = 0x004,
  ET_KeyboardUp = 0x008,

  ET_MouseButtonUp = 0x010,
  ET_MouseButtonDown = 0x020,
  ET_MouseMotion = 0x040,

  ET_JoystickAxisMotion = 0x080,
  ET_JoystickButtonDown = 0x100,
  ET_JoystickButtonUp = 0x200,
  ET_JoystickHat = 0x400,

  ET_KeyboardEvents = ET_KeyboardDown | ET_KeyboardUp,
  ET_MouseEvents = ET_MouseButtonDown | ET_MouseButtonUp | ET_MouseMotion,
  ET_JoystickEvents = ET_JoystickAxisMotion | ET_JoystickButtonDown | ET_JoystickButtonUp | ET_JoystickHat,

  ET_InputEvents = ET_KeyboardEvents | ET_MouseEvents | ET_JoystickEvents,

  ET_AllEvents = ET_WindowResize | ET_FontResize | ET_InputEvents
};

class Event
{
    Event() : _type(ET_WindowResize) {}
  public:
    Event(EventType pType) : _type(pType), _pass(true) {}
    virtual ~Event() {}

    inline EventType type() const
      { return _type; }

    inline bool pass() const
      { return _pass; }

    inline void stop()
      { _pass = false; }

  private:
    EventType _type;
    bool _pass;
};

class WindowResizeEvent : public Event
{
  public:
    WindowResizeEvent(const Size &pPreviousSize, const Size &pNewSize)
      : Event(ET_WindowResize),
        _previousSize(pPreviousSize),
        _newSize(pNewSize) {}

    inline Size previousSize() const
      { return _previousSize; }

    inline Size newSize() const
      { return _newSize; }

  private:
    const Size _previousSize, _newSize;
};

class FontResizeEvent : public Event
{
  public:
    FontResizeEvent() : Event(ET_FontResize) { }
};


template<EventType Type, class BaseSDLEvent>
class SDLBasedEvent : public Event
{
  public:
    SDLBasedEvent(const BaseSDLEvent &pBaseEvent)
      : Event(Type), _baseEvent(pBaseEvent) {}

    inline const BaseSDLEvent& event() const
    {
      return _baseEvent;
    }

  private:
    BaseSDLEvent _baseEvent;
};

typedef SDLBasedEvent<ET_KeyboardDown, SDL_KeyboardEvent> KeyboardDownEvent;
typedef SDLBasedEvent<ET_KeyboardUp, SDL_KeyboardEvent> KeyboardUpEvent;

typedef SDLBasedEvent<ET_MouseButtonDown, SDL_MouseButtonEvent> MouseButtonDownEvent;
typedef SDLBasedEvent<ET_MouseButtonUp, SDL_MouseButtonEvent> MouseButtonUpEvent;
typedef SDLBasedEvent<ET_MouseMotion, SDL_MouseMotionEvent> MouseMotionEvent;

typedef SDLBasedEvent<ET_JoystickAxisMotion, SDL_JoyAxisEvent> JoystickAxisMotionEvent;
typedef SDLBasedEvent<ET_JoystickButtonDown, SDL_JoyButtonEvent> JoystickButtonDownEvent;
typedef SDLBasedEvent<ET_JoystickButtonUp, SDL_JoyButtonEvent> JoystickButtonUpEvent;
typedef SDLBasedEvent<ET_JoystickHat, SDL_JoyHatEvent> JoystickHatEvent;
