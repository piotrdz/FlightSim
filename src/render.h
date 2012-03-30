/***************************************************************************
 *   Copyright (C) 2011-2012 by Piotr Dziwinski                            *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* render.h
    Contains the Render class, which is responsible for rendering and
    managing objects of class Widget. */

#pragma once

#include "config.h"

#include "common.h"
#include "widget.h"
#include "label.h"

#include <vector>

class Menu;
class MapDialog;
class GameDialog;
class Simulation;
class SettingsDialog;
class Console;

class Render : public Widget
{
  public:
    Render();
    virtual ~Render();

    inline static Render* instance()
      { return _instance; }

    inline Rect viewport() const
      { return _viewport; }

    inline float fps() const
      { return _fps; }

    inline bool fpsVisible() const
      { return _fpsLabel->visible(); }
    inline void setFPSVisible(bool pDisplayFPS)
      { _fpsLabel->setVisible(pDisplayFPS); }

    void loadSettings();

    virtual void init();
    virtual void render();
    virtual void update();

    void begin3D(float fov, float zNear, float zFar);
    void end3D();

  private:
    static Render *_instance;
    Rect _viewport;

    Menu *_mainMenu;

    MapDialog *_mapDialog;
    GameDialog *_gameDialog;

    Simulation *_simulation;

    SettingsDialog *_settingsDialog;

    Console *_console;
    std::vector<int> _consoleSaveEvents;

    Widget *_childEventSender;
    int _childEventParameter;

    Label *_fpsLabel;
    Timer _fpsTimer;
    int _frames;
    float _fps;

    virtual void windowResizeEvent(WindowResizeEvent *e);
    virtual void resizeEvent();

    virtual void keyboardDownEvent(KeyboardDownEvent *e);

    virtual void childEvent(Widget *sender, int parameter);

    void consoleCommand(const std::string &command);
};
