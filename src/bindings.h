/***************************************************************************
 *   Copyright (C) 2011-2012 by Piotr Dziwinski                            *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

/* bindings.h
   Contains the classes: KeyBinding, JoystickAxisBinding
   and JoystickButtonBinding, which are responsible for assigning controls
   to keys and joystick, as well as BindingManager, which manages all
   bindings. */

#pragma once

#include "object.h"

#include <string>
#include <vector>
#include <map>

class KeyboardEvent;

class KeyBinding
{
  public:
    KeyBinding();
    KeyBinding(int pKeysym);

    inline bool valid() const
      { return _valid; }

    inline int keysym() const
      { return _keysym; }

    std::string name() const;

    bool set(int pKeysym, bool valid = true);

    inline bool check(int pKeysym) const
    {
      if (!_valid) return false;
      return _keysym == pKeysym;
    }

  private:
    bool _valid;
    int _keysym;

    static std::string translateSupportedKeysym(int pKeysym);
};

class JoystickAxisBinding
{
  public:
    JoystickAxisBinding(int pAxis = -1, bool pInverted = false)
      : _axis(pAxis), _inverted(pInverted) {}

    inline void set(int pAxis, bool pInverted)
      { _axis = pAxis; _inverted = pInverted; }

    inline int axis() const
      { return _axis; }

    inline bool valid() const
      { return _axis >= 0; }

    inline bool inverted() const
      { return _inverted; }

    std::string name() const;

  private:
    int _axis;
    bool _inverted;
};

class JoystickButtonBinding
{
  public:
    JoystickButtonBinding(int pButton = -1)
      : _button(pButton) {}

    inline void set(int pButton)
      { _button = pButton; }

    inline bool valid() const
      { return _button >= 0; }

    inline int button() const
      { return _button; }

    std::string name() const;

  private:
    int _button;
};

class BindingManager : public Object
{
  public:
    BindingManager();

    static BindingManager* instance()
      { return _instance; }

    void loadSettings();

    void registerKey(const std::string &pName,
                     const KeyBinding &pBinding);
    std::vector<std::string> registeredKeys() const;
    const KeyBinding& findKey(const std::string &pName) const;
    bool setKey(const std::string &pName, int pKeysym, bool pValid = true);

    void registerJoystickAxis(const std::string &pName,
                              const JoystickAxisBinding &pBinding);
    const JoystickAxisBinding& findJoystickAxis(const std::string &pName) const;
    bool setJoystickAxis(const std::string &pName, int pAxis, bool pInverted);

    void registerJoystickButton(const std::string &pName,
                                const JoystickButtonBinding &pBinding);
    const JoystickButtonBinding& findJoystickButton(const std::string &pName) const;
    bool setJoystickButton(const std::string &pName, int pButton);

  private:
    static BindingManager *_instance;
    KeyBinding _invalidKey;
    JoystickButtonBinding _invalidJoystickButton;
    JoystickAxisBinding _invalidJoystickAxis;

    std::map<std::string, KeyBinding> _keyMap;
    std::map<std::string, JoystickAxisBinding> _joystickAxisMap;
    std::map<std::string, JoystickButtonBinding> _joystickButtonMap;
};
