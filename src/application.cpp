/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

/* application.cpp
   Zawiera implementację klasy Application. */

#include "application.h"

#include "render.h"
#include "settings.h"
#include "bindings.h"
#include "fontengine.h"
#include "decorator.h"
#include "filemanager.h"
#include "console.h"

#include <iostream>
#include <sstream>
#include <cassert>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

using namespace std;


Application* Application::_instance = NULL;

Application::Application(int argc, char **argv)
    : Object("Application")
{
  assert(_instance == NULL);

  _instance = this;

  _argc = argc;
  _argv = argv;

  _applicationName = "FlightSim";
  _applicationVersion = "1.0";

  // Domyślne wartości
  _windowSettings.size = Size(1024, 768);
  _windowSettings.bpp = 32;
  _windowSettings.fullScreen = false;
  _windowSettings.caption = _applicationName + " " + _applicationVersion;

  _quitCode = 0;

  _quit = _windowSettingsChanged = false;

  _surface = NULL;
  _joystick = NULL;
  _joystickDevice = 0;

  _settings = new Settings;

  _bindingManager = new BindingManager;

  _fontManager = new FontManager;

  _decorator = new Decorator;

  _render = new Render;

  FileManager::instance()->registerFile("WindowIcon", "data/icon.png");

  _settings->registerSetting<int>("ResolutionX", 1024);
  _settings->registerSetting<int>("ResolutionY", 768);
  _settings->registerSetting<bool>("Fullscreen", false);
  _settings->registerSetting<bool>("Multisampling", false);
  _settings->registerSetting<int>("JoystickDevice", 0);
}

Application::~Application()
{
  assert(_surface == NULL);

  delete _render;
  _render = NULL;

  delete _decorator;
  _decorator = NULL;

  delete _fontManager;
  _fontManager = NULL;

  delete _bindingManager;
  _bindingManager = NULL;

  delete _settings;
  _settings = NULL;

  _instance = NULL;
}

void Application::loadSettings()
{
  _windowSettings.size.w = _settings->setting<int>("ResolutionX");
  _windowSettings.size.h = _settings->setting<int>("ResolutionY");
  _windowSettings.fullScreen = _settings->setting<bool>("Fullscreen");
  _windowSettings.multisampling = _settings->setting<bool>("Multisampling");

  _joystickDevice = _settings->setting<int>("JoystickDevice");
}

void Application::parseArgs()
{
  if (_argc <= 1)
    return;

  stringstream stream;

  bool skip = false;

  for (int i = 1; i < _argc; ++i)
  {
    if (skip)
    {
      skip = false;
      continue;
    }

    stream.str(_argv[i]);

    if (stream.str() == "-fs")
    {
      _windowSettings.fullScreen = true;
    }
    else if (stream.str() == "-nfs")
    {
      _windowSettings.fullScreen = false;
    }
    else if (stream.str() == "-v")
    {
      cout << _applicationName << " wersja " << _applicationVersion << endl;
      quit(0);
      return;
    }
    else if (stream.str() == "-h")
    {
      cout << "Użycie: " << _argv[0] << " [-size szerXwys -b bpp (-fs|-nfs)]" << endl;
      quit(0);
      return;
    }
    else if (stream.str() == "-size")
    {
      if (i >= _argc - 1)
      {
        cout << "Brak argumentu do -size!" << endl;
        quit(1);
        return;
      }

      stream.clear();
      stream.str(_argv[i + 1]);
      int width = 0;
      stream >> width;
      char x = '\0';
      stream >> x;
      int height = 0;
      stream >> height;

      if ((width <= 0) || (height <= 0))
      {
        cout << "Błędny argument do -size!" << endl;
        quit(1);
        return;
      }

      _windowSettings.size.w = width;
      _windowSettings.size.h = height;

      skip = true;
    }
    else if (stream.str() == "-b")
    {
      if (i >= _argc - 1)
      {
        cout << "Brak argumentu do -b!" << endl;
        quit(1);
        return;
      }

      stream.clear();
      stream.str(_argv[i + 1]);
      int bpp = 0;
      stream >> bpp;

      if (bpp <= 0)
      {
        cout << "Błędny argument do -b!" << endl;
        quit(1);
        return;
      }

      _windowSettings.bpp = bpp;

      skip = true;
    }
    else
    {
      cout << "Zły argument: " << stream.str() << endl;
      quit(1);
      return;
    }
  }
}

void Application::quit(int code)
{
  _quit = true;
  _quitCode = code;
}

void Application::print(const std::string &module,
                        const std::string &message) const
{
  cout << module << ":: " << message << endl;
  if (Console::instance() != NULL)
  {
    stringstream line;
    line << module << ":: " << message;
    Console::instance()->print(line.str());
  }
}

void Application::init()
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
  {
    Object::print("Błąd w SDL_Init(): " + string(SDL_GetError()));
    quit(1);
    return;
  }

  const SDL_VideoInfo *videoInfo = SDL_GetVideoInfo();

  if (!videoInfo)
  {
    Object::print("Błąd w SDL_GetVideoInfo(): " + string(SDL_GetError()));
    quit(1);
    return;
  }

  _videoFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_RESIZABLE;

  if (videoInfo->hw_available)
    _videoFlags |= SDL_HWSURFACE;
  else
    _videoFlags |= SDL_SWSURFACE;

  if (videoInfo->blit_hw)
    _videoFlags |= SDL_HWACCEL;

  if (_windowSettings.fullScreen)
    _videoFlags |= SDL_FULLSCREEN;

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  if (_windowSettings.multisampling)
  {
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
  }


  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0)
  {
    Object::print("Błąd w IMG_Init()!");
    quit(1);
    return;
  }

  if (FileManager::instance()->canRead("WindowIcon"))
  {
    string iconFileName = FileManager::instance()->fileName("WindowIcon");
    SDL_Surface *iconSurface = IMG_Load(iconFileName.c_str());

    if (iconSurface)
      SDL_WM_SetIcon(iconSurface, NULL);

    SDL_FreeSurface(iconSurface);
  }

  _surface = SDL_SetVideoMode((int)_windowSettings.size.w,
                              (int)_windowSettings.size.h,
                              _windowSettings.bpp,
                              _videoFlags);

  if (!_surface)
  {
    Object::print("Błąd w SDL_SetVideoMode(): " + string(SDL_GetError()));
    quit(1);
    return;
  }

  SDL_WM_SetCaption(_windowSettings.caption.c_str(), _applicationName.c_str());

  SDL_EnableUNICODE(1);

  if (SDL_NumJoysticks() > 0)
  {
    SDL_JoystickEventState(SDL_ENABLE);
    _joystick = SDL_JoystickOpen(_joystickDevice);
  }
}

void Application::changeWindowSettings()
{
  if (_newWindowSettings.fullScreen)
    _videoFlags |= SDL_FULLSCREEN;
  else
    _videoFlags &= ~SDL_FULLSCREEN;

  _surface = SDL_SetVideoMode((int)_newWindowSettings.size.w,
                              (int)_newWindowSettings.size.h,
                              _newWindowSettings.bpp,
                              _videoFlags);

  if (!_surface)
  {
    Object::print("Błąd w SDL_SetVideoMode(): " + string(SDL_GetError()));
    quit(1);
    return;
  }

  SDL_WM_SetCaption(_newWindowSettings.caption.c_str(), _applicationName.c_str());

  WindowSettings oldWindowSettings = _windowSettings;
  _windowSettings = _newWindowSettings;

  if (_windowSettings.size != oldWindowSettings.size)
  {
    WindowResizeEvent event(oldWindowSettings.size, _windowSettings.size);
    _fontManager->windowResized(&event);
    _decorator->windowResized(&event);
    _render->sendEvent(&event);
  }

  _windowSettingsChanged = false;
}

int Application::execute()
{
  if (_quit)
    return _quitCode;

  _settings->load();

  if (_quit)
    return _quitCode;

  loadSettings();
  _bindingManager->loadSettings();
  _render->loadSettings();

  parseArgs();

  if (_quit)
    return _quitCode;

  init();

  if (!_quit)
  {
    _render->init();

    WindowResizeEvent firstResize(Size(0, 0), _windowSettings.size);
    _render->sendEvent(&firstResize);
  }

  SDL_Event event;

  while (!_quit)
  {
    // Obsługa zdarzeń z SDL
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        // Użytkownik zmienił rozmiar okna
        case SDL_VIDEORESIZE:
        {
          _newWindowSettings = _windowSettings;
          _newWindowSettings.size.w = event.resize.w;
          _newWindowSettings.size.h = event.resize.h;
          _windowSettingsChanged = true;
          break;
        }

        // Użytkownik wcisnął klawisz
        case SDL_KEYDOWN:
        {
          KeyboardDownEvent e(event.key);
          _render->sendEvent(&e);
          break;
        }

        // Użytkownik zwolnił klawisz
        case SDL_KEYUP:
        {
          KeyboardUpEvent e(event.key);
          _render->sendEvent(&e);
          break;
        }

        // Użytkownik wcisnął przycisk myszy
        case SDL_MOUSEBUTTONDOWN:
        {
          MouseButtonDownEvent e(event.button);
          _render->sendEvent(&e);
          break;
        }

        // Użytkownik zwolnił przycisk myszy
        case SDL_MOUSEBUTTONUP:
        {
          MouseButtonUpEvent e(event.button);
          _render->sendEvent(&e);
          break;
        }

        // Użytkownik przesunął myszę
        case SDL_MOUSEMOTION:
        {
          MouseMotionEvent e(event.motion);
          _render->sendEvent(&e);
          break;
        }

        // Użytkownik poruszył joystickiem
        case SDL_JOYAXISMOTION:
        {
          JoystickAxisMotionEvent e(event.jaxis);
          _render->sendEvent(&e);
          break;
        }

        // Użytkownik wcisnął przycisk joysticka
        case SDL_JOYBUTTONDOWN:
        {
          JoystickButtonDownEvent e(event.jbutton);
          _render->sendEvent(&e);
          break;
        }

        // Użytkownik zwolnił przycisk joysticka
        case SDL_JOYBUTTONUP:
        {
          JoystickButtonUpEvent e(event.jbutton);
          _render->sendEvent(&e);
          break;
        }

        // Użytkownik zmienił ustawienie "grzybka" joysticka
        case SDL_JOYHATMOTION:
        {
          JoystickHatEvent e(event.jhat);
          _render->sendEvent(&e);
          break;
        }

        // Użytkownik zamknął okno
        case SDL_QUIT:
        {
          Object::print("Okno zamknięte");
          _quit = true;
          break;
        }

        default:
          break;
      }
    }

    if (_settings->settingsChanged())
    {
      _bindingManager->loadSettings();
      _render->loadSettings();
      _settings->clearSettingsChanged();
    }

    if (_windowSettingsChanged)
      changeWindowSettings();

    if (_quit)
      break;

    _render->render();

    SDL_GL_SwapBuffers();

    if (_quit)
      break;

    _render->update();

    if (_quit)
      break;
  }

  if (_quitCode == 0)
  {
    _settings->save();
  }

  if (_windowSettings.fullScreen)
    SDL_WM_ToggleFullScreen(_surface);

  if (_joystick != NULL)
  {
    SDL_JoystickClose(_joystick);
    _joystick = NULL;
  }

  SDL_FreeSurface(_surface);
  _surface = NULL;

  IMG_Quit();

  SDL_Quit();

  return _quitCode;
}
