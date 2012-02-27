/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

/* application.h
   Contains the Application class, which is responsible for interfacing
   with SDL and managing the program window. */

#pragma once

#include "config.h"

#include "object.h"
#include "common.h"

class Settings;
class BindingManager;
class FontManager;
class Decorator;
class Render;

struct WindowSettings
{
    Size size;
    int bpp;
    bool fullScreen;
    bool multisampling;
    std::string caption;

    WindowSettings()
    {
        bpp = 0;
        fullScreen = false;
    }
};

class Application : public Object
{
  public:
    Application(int argc, char *argv[]);
    virtual ~Application();

    inline static Application* instance()
      { return _instance; }

    inline WindowSettings windowSettings() const
      { return _windowSettings; }
    inline void setWindowSettings(const WindowSettings &settings)
    {
      _newWindowSettings = settings;
      _windowSettingsChanged = true;
    }

    inline std::string applicationName() const
      { return _applicationName; }

    inline std::string applicationVersion() const
      { return _applicationVersion; }

    void loadSettings();

    int execute();

    void quit(int pCode = 0);

    void print(const std::string &module, const std::string &message) const;

  private:
    static Application *_instance;

    int _argc;
    char **_argv;

    std::string _applicationName, _applicationVersion;
    WindowSettings _windowSettings, _newWindowSettings;
    int _quitCode;

    bool _quit;
    bool _windowSettingsChanged;

    int _videoFlags;
    SDL_Surface *_surface;
    SDL_Joystick *_joystick;
    int _joystickDevice;

    Settings *_settings;
    BindingManager *_bindingManager;
    FontManager *_fontManager;
    Decorator *_decorator;
    Render *_render;

    void parseArgs();
    void init();
    void changeWindowSettings();
};
