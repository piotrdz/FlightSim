/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* bindings.cpp
    Zawiera implementację klas z bindings.h. */

#include "bindings.h"

#include "settings.h"

#include <SDL/SDL_keysym.h>

#include <sstream>
#include <cassert>

using namespace std;


string KeyBinding::translateSupportedKeysym(int pKeysym)
{
  switch (pKeysym)
  {
    case SDLK_ESCAPE:
      return "escape";
    case SDLK_BACKSPACE:
      return "backspace";
    case SDLK_TAB:
      return "tab";
    case SDLK_RETURN:
      return "return";
    case SDLK_SPACE:
      return "space";
    case SDLK_RSHIFT:
      return "rshift";
    case SDLK_LSHIFT:
      return "lshift";
    case SDLK_RCTRL:
      return "rctrl";
    case SDLK_LCTRL:
      return "lctrl";
    case SDLK_RALT:
      return "ralt";
    case SDLK_LALT:
      return "lalt";
    case SDLK_RMETA:
      return "rmeta";
    case SDLK_LMETA:
      return "lmeta";
    case SDLK_LSUPER:
      return "lsuper";
    case SDLK_RSUPER:
      return "rsuper";
    case SDLK_DELETE:
      return "delete";
    case SDLK_KP0:
      return "kp0";
    case SDLK_KP1:
      return "kp1";
    case SDLK_KP2:
      return "kp2";
    case SDLK_KP3:
      return "kp3";
    case SDLK_KP4:
      return "kp4";
    case SDLK_KP5:
      return "kp5";
    case SDLK_KP6:
      return "kp6";
    case SDLK_KP7:
      return "kp7";
    case SDLK_KP8:
      return "kp8";
    case SDLK_KP9:
      return "kp9";
    case SDLK_KP_PERIOD:
      return "kp_period";
    case SDLK_KP_DIVIDE:
      return "kp_divide";
    case SDLK_KP_MULTIPLY:
      return "kp_multiply";
    case SDLK_KP_MINUS:
      return "kp_minus";
    case SDLK_KP_PLUS:
      return "kp_plus";
    case SDLK_KP_ENTER:
      return "kp_enter";
    case SDLK_KP_EQUALS:
      return "kp_equals";
    case SDLK_UP:
      return "up";
    case SDLK_DOWN:
      return "down";
    case SDLK_RIGHT:
      return "right";
    case SDLK_LEFT:
      return "left";
    case SDLK_INSERT:
      return "insert";
    case SDLK_HOME:
      return "home";
    case SDLK_END:
      return "end";
    case SDLK_PAGEUP:
      return "pageup";
    case SDLK_PAGEDOWN:
      return "pagedown";
    case SDLK_F1:
      return "f1";
    case SDLK_F2:
      return "f2";
    case SDLK_F3:
      return "f3";
    case SDLK_F4:
      return "f4";
    case SDLK_F5:
      return "f5";
    case SDLK_F6:
      return "f6";
    case SDLK_F7:
      return "f7";
    case SDLK_F8:
      return "f8";
    case SDLK_F9:
      return "f9";
    case SDLK_F10:
      return "f10";
    case SDLK_F11:
      return "f11";
    case SDLK_F12:
      return "f12";
    case SDLK_F13:
      return "f13";
    case SDLK_F14:
      return "f14";
    case SDLK_F15:
      return "f15";
    case SDLK_EXCLAIM:
      return "!";
    case SDLK_QUOTEDBL:
      return "\"";
    case SDLK_HASH:
      return "#";
    case SDLK_DOLLAR:
      return "$";
    case SDLK_AMPERSAND:
      return "&";
    case SDLK_QUOTE:
      return "'";
    case SDLK_LEFTPAREN:
      return "(";
    case SDLK_RIGHTPAREN:
      return ")";
    case SDLK_ASTERISK:
      return "*";
    case SDLK_PLUS:
      return "+";
    case SDLK_COMMA:
      return ",";
    case SDLK_MINUS:
      return "-";
    case SDLK_PERIOD:
      return ".";
    case SDLK_SLASH:
      return "/";
    case SDLK_0:
      return "0";
    case SDLK_1:
      return "1";
    case SDLK_2:
      return "2";
    case SDLK_3:
      return "3";
    case SDLK_4:
      return "4";
    case SDLK_5:
      return "5";
    case SDLK_6:
      return "6";
    case SDLK_7:
      return "7";
    case SDLK_8:
      return "8";
    case SDLK_9:
      return "9";
    case SDLK_COLON:
      return ":";
    case SDLK_SEMICOLON:
      return ";";
    case SDLK_LESS:
      return "<";
    case SDLK_EQUALS:
      return "=";
    case SDLK_GREATER:
      return ">";
    case SDLK_QUESTION:
      return "?";
    case SDLK_AT:
      return "@";
    case SDLK_LEFTBRACKET:
      return "[";
    case SDLK_BACKSLASH:
      return "\\";
    case SDLK_RIGHTBRACKET:
      return "]";
    case SDLK_CARET:
      return "^";
    case SDLK_UNDERSCORE:
      return "_";
    case SDLK_BACKQUOTE:
      return "`";
    case SDLK_a:
      return "a";
    case SDLK_b:
      return "b";
    case SDLK_c:
      return "c";
    case SDLK_d:
      return "d";
    case SDLK_e:
      return "e";
    case SDLK_f:
      return "f";
    case SDLK_g:
      return "g";
    case SDLK_h:
      return "h";
    case SDLK_i:
      return "i";
    case SDLK_j:
      return "j";
    case SDLK_k:
      return "k";
    case SDLK_l:
      return "l";
    case SDLK_m:
      return "m";
    case SDLK_n:
      return "n";
    case SDLK_o:
      return "o";
    case SDLK_p:
      return "p";
    case SDLK_q:
      return "q";
    case SDLK_r:
      return "r";
    case SDLK_s:
      return "s";
    case SDLK_t:
      return "t";
    case SDLK_u:
      return "u";
    case SDLK_v:
      return "v";
    case SDLK_w:
      return "w";
    case SDLK_x:
      return "x";
    case SDLK_y:
      return "y";
    case SDLK_z:
      return "z";
    default:
      return "";
  }
  return "";
}

KeyBinding::KeyBinding()
{
  _keysym = 0;
  _valid = false;
}

KeyBinding::KeyBinding(int pKeysym)
{
  assert(! translateSupportedKeysym(pKeysym).empty());

  _keysym = pKeysym;
  _valid = true;
}

bool KeyBinding::set(int pKeysym, bool valid)
{
  if (!valid)
  {
    _valid = false;
    return true;
  }

  if (translateSupportedKeysym(pKeysym).empty())
    return false;

  _keysym = pKeysym;
  _valid = true;

  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

std::string KeyBinding::name() const
{
  if (_valid)
    return "(brak)";

  return translateSupportedKeysym(_keysym);
}

string JoystickAxisBinding::name() const
{
  if (!valid())
    return "(brak)";

  stringstream s;
  s << "Oś " << _axis;
  if (_inverted)
    s << " odwrócona";

  return s.str();
}

string JoystickButtonBinding::name() const
{
  if (!valid())
    return "(brak)";

  stringstream s;
  s << "Przycisk " << _button;
  return s.str();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

BindingManager* BindingManager::_instance = NULL;

BindingManager::BindingManager() : Object("BindingManager")
{
  assert(_instance == NULL);

  _instance = this;
}

void BindingManager::loadSettings()
{
  Settings *s = Settings::instance();

  for (map<string, KeyBinding>::iterator it = _keyMap.begin();
       it != _keyMap.end(); ++it)
  {
    int keysym = s->setting<int>(string("KeyBinding_") + (*it).first + "_Key");
    bool valid = s->setting<bool>(string("KeyBinding_") + (*it).first + "_Valid");
    (*it).second.set(keysym, valid);
  }

  for (map<string, JoystickAxisBinding>::iterator it = _joystickAxisMap.begin();
       it != _joystickAxisMap.end(); ++it)
  {
    int axis = s->setting<int>(string("JoystickAxisBinding_") + (*it).first + "_Axis");
    bool inverted = s->setting<bool>(string("JoystickAxisBinding_") + (*it).first + "_Inverted");
    (*it).second.set(axis, inverted);
  }

  for (map<string, JoystickButtonBinding>::iterator it = _joystickButtonMap.begin();
       it != _joystickButtonMap.end(); ++it)
  {
    int button = s->setting<int>(string("JoystickButtonBinding_") + (*it).first);
    (*it).second.set(button);
  }
}

void BindingManager::registerKey(const std::string& pName,
                                 const KeyBinding& binding)
{
  _keyMap[pName] = binding;
  Settings::instance()->registerSetting<int>(string("KeyBinding_") +
                          pName + "_Key", binding.keysym());
  Settings::instance()->registerSetting<bool>(string("KeyBinding_") +
                          pName + "_Valid", binding.valid());
}

const KeyBinding& BindingManager::findKey(const std::string& pName) const
{
  map<string, KeyBinding>::const_iterator it = _keyMap.find(pName);
  if (it == _keyMap.end())
  {
    print(string("Brak klucza '") + pName + "'");
    return _invalidKey;
  }
  return (*it).second;
}

bool BindingManager::setKey(const std::string &pName, int pKeysym, bool pValid)
{
  map<string, KeyBinding>::iterator it = _keyMap.find(pName);
  if (it == _keyMap.end())
  {
    print(string("Brak klucza '") + pName + "'");
    return false;
  }

  bool result = (*it).second.set(pKeysym, pValid);
  if (result)
  {
    Settings::instance()->setSetting<int>(string("KeyBinding_") +
                            pName + "_Key", pKeysym);
    Settings::instance()->setSetting<bool>(string("KeyBinding_") +
                            pName + "_Valid", pValid);
  }

  return result;
}

void BindingManager::registerJoystickAxis(const std::string& pName,
                                          const JoystickAxisBinding& binding)
{
  _joystickAxisMap[pName] = binding;
  Settings::instance()->registerSetting<int>(string("JoystickAxisBinding_") +
                          pName + "_Axis", binding.axis());
  Settings::instance()->registerSetting<bool>(string("JoystickAxisBinding_") +
                          pName + "_Inverted", binding.inverted());
}

const JoystickAxisBinding& BindingManager::findJoystickAxis(const std::string& pName) const
{
  map<string, JoystickAxisBinding>::const_iterator it = _joystickAxisMap.find(pName);
  if (it == _joystickAxisMap.end())
  {
    print(string("Brak klucza '") + pName + "'");
    return _invalidJoystickAxis;
  }
  return (*it).second;
}

bool BindingManager::setJoystickAxis(const std::string &pName, int pAxis, bool pInverted)
{
  map<string, JoystickAxisBinding>::iterator it = _joystickAxisMap.find(pName);
  if (it == _joystickAxisMap.end())
  {
    print(string("Brak klucza '") + pName + "'");
    return false;
  }

  (*it).second.set(pAxis, pInverted);

  Settings::instance()->setSetting<int>(string("JoystickAxisBinding_") +
                            pName + "_Axis", pAxis);
  Settings::instance()->setSetting<bool>(string("JoystickAxisBinding_") +
                            pName + "_Inverted", pInverted);

  return true;
}

void BindingManager::registerJoystickButton(const std::string& pName,
                                            const JoystickButtonBinding& binding)
{
  _joystickButtonMap[pName] = binding;
  Settings::instance()->registerSetting<int>(string("JoystickButtonBinding_") +
                          pName, binding.button());
}

const JoystickButtonBinding& BindingManager::findJoystickButton(const std::string& pName) const
{
  map<string, JoystickButtonBinding>::const_iterator it = _joystickButtonMap.find(pName);
  if (it == _joystickButtonMap.end())
  {
    print(string("Brak klucza '") + pName + "'");
    return _invalidJoystickButton;
  }
  return (*it).second;
}

bool BindingManager::setJoystickButton(const std::string &pName, int pButton)
{
  map<string, JoystickButtonBinding>::iterator it = _joystickButtonMap.find(pName);
  if (it == _joystickButtonMap.end())
  {
    print(string("Brak klucza '") + pName + "'");
    return false;
  }

  (*it).second.set(pButton);

  Settings::instance()->setSetting<int>(string("JoystickButtonBinding_") +
                            pName, pButton);

  return true;
}
