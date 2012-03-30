/***************************************************************************
 *   Copyright (C) 2011-2012 by Piotr Dziwinski                            *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* render.cpp
    Contains the implementation of the Render class. */

#include "render.h"

#include "application.h"
#include "decorator.h"
#include "menu.h"
#include "settings.h"
#include "bindings.h"
#include "map.h"
#include "mapdialog.h"
#include "gamedialog.h"
#include "simulation.h"
#include "settingsdialog.h"
#include "console.h"

#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iostream>
#include <cassert>

#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;


Render* Render::_instance = NULL;

Render::Render() : Widget(NULL, "Render")
{
  assert(_instance == NULL);
  _instance = this;

  _childEventSender = NULL;

  _viewport = Rect(Point(0, 0), Application::instance()->windowSettings().size);

  _fpsTimer.setIntervalMsec(500);
  _fps = 1.0f;
  _frames = 0;

  setEventMask(ET_AllEvents);
  setVisible(true);

  _mapDialog = new MapDialog(this);

  _gameDialog = new GameDialog(this);

  _simulation = new Simulation(this, "Simulation");

  _settingsDialog = new SettingsDialog(this);

  _console = new Console(this);

  Settings::instance()->registerSetting<bool>("FPS", true);

  BindingManager *b = BindingManager::instance();
  b->registerKey("Console", KeyBinding(SDLK_BACKQUOTE));
  b->registerKey("ToggleFPS", KeyBinding(SDLK_F1));
  b->registerKey("Quit", KeyBinding(SDLK_F4));
}

Render::~Render()
{
  _instance = NULL;

  Player::destroyModel();

  _mainMenu = NULL;
  _simulation = NULL;
  _mapDialog = NULL;
  _gameDialog = NULL;
  _settingsDialog = NULL;
  _console = NULL;
  _fpsLabel = NULL;
}

void Render::loadSettings()
{
  _fpsLabel->setVisible(Settings::instance()->setting<bool>("FPS"));
}

void Render::init()
{
  vector<string> mainMenuItems;
  mainMenuItems.push_back(_("Simulation"));
  mainMenuItems.push_back(_("Game with enemies"));
  mainMenuItems.push_back(_("Settings"));
  mainMenuItems.push_back(_("Exit"));

  _mainMenu = new Menu(this, "FlightSim", mainMenuItems, true, "Mode_MainMenu");
  _mainMenu->show();

  _fpsLabel = new Label(this, "FPS: 0.0",
                        Decorator::instance()->getFont(FT_Small),
                        AL_Left | AL_Bottom, false,
                        Color(1.0f, 1.0f, 1.0f, 1.0f),
                        false, "FPS_Label");
  _fpsLabel->show();


  Color c = Decorator::instance()->getColor(C_InterfaceBackground);
  glClearColor(c.r, c.g, c.b, c.a);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_DEPTH_TEST);

  glEnable(GL_MULTISAMPLE);

  glShadeModel(GL_SMOOTH);

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  Player::initModel();
  Map::initFunctions();

  initChildren();
}

void Render::render()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  renderChildren();
}

void Render::update()
{
  updateChildren();

  ++_frames;
  if (_fpsTimer.checkTimeout())
  {
    _fps = (_frames * 1e9f) / _fpsTimer.timeoutDifference();

    if (_fpsLabel->visible())
    {
      ostringstream stream;
      stream.precision(2);
      stream.setf(ios_base::fixed | ios_base::showpoint);
      stream << "FPS: " << (float)_fps;
      _fpsLabel->setText(stream.str());

      _frames = 0;
    }
  }

  if ((_childEventSender == _mainMenu) && (_childEventParameter == Menu::ItemChosen))
  {
    _mainMenu->hide();

    switch (_mainMenu->index())
    {
      // Simulation
      case 0:
      {
        print("Menu - simulation");

        _mapDialog->show();
        _simulation->setType(Simulation::Simulation_Normal);
        break;
      }
      // Game
      case 1:
      {
        print("Menu - game");

        _mapDialog->show();
        _simulation->setType(Simulation::Simulation_Game);
        break;
      }
      // Settings
      case 2:
      {
        print("Menu - settings");

        _settingsDialog->show();
        break;
      }
      // Exit
      case 3:
      {
        print("Menu - exit");
        Application::instance()->quit(0);
        break;
      }
      default: {}
    }
  }
  else if (_childEventSender == _simulation)
  {
    if (_childEventParameter == Simulation::Action_EndGame)
    {
      _simulation->hide();
      _mainMenu->show();
    }
    else if (_childEventParameter == Simulation::Action_Settings)
    {
      _settingsDialog->show();
    }
  }
  else if (_childEventSender == _mapDialog)
  {
    _mapDialog->hide();
    if (_childEventParameter == MapDialog::Okeyed)
    {
      _simulation->setFractalOptions(_mapDialog->fractalOptions());
      _simulation->setScale(_mapDialog->scale());
      _simulation->reset();
      if (_simulation->type() == Simulation::Simulation_Normal)
        _simulation->show();
      else
        _gameDialog->show();
    }
    else
    {
      _mainMenu->show();
    }
  }
  else if (_childEventSender == _gameDialog)
  {
    _gameDialog->hide();
    if (_childEventParameter == GameDialog::Okeyed)
    {
      _simulation->setPlayerAmmo(_gameDialog->playerAmmo());
      _simulation->addEnemies(_gameDialog->enemyCount(), _gameDialog->enemyActions());
      _simulation->show();
    }
    else
    {
      _mapDialog->show();
    }
  }
  else if (_childEventSender == _settingsDialog)
  {
    _settingsDialog->hide();
    if (_simulation->visible())
      _simulation->settingsDialogFinished();
    else
      _mainMenu->show();
  }
  else if (_childEventSender == _console)
  {
    consoleCommand(_console->command());

    _console->clearCommand();
  }

  _childEventSender = NULL;
  _childEventParameter = -1;
}

void Render::begin3D(float fov, float zNear, float zFar)
{
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fov, _viewport.w / _viewport.h, zNear, zFar);

  glMatrixMode(GL_MODELVIEW);

  glEnable(GL_DEPTH_TEST);
}

void Render::end3D()
{
  glDisable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void Render::windowResizeEvent(WindowResizeEvent *e)
{
  _viewport = Rect(Point(0, 0), e->newSize());
  glViewport((int)_viewport.x, (int)_viewport.y,
             (int)_viewport.w, (int)_viewport.h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluOrtho2D(0, _viewport.w, _viewport.h, 0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glShadeModel(GL_SMOOTH);

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  setGeometry(_viewport);
}

void Render::resizeEvent()
{
  _console->setGeometry(Rect(geometry().position(),
                             Size(geometry().w, geometry().h / 2.0f)));

  _fpsLabel->setGeometry(geometry());

  Rect mainMenuArea;
  mainMenuArea.w = 0.5f * geometry().w;
  mainMenuArea.h = 0.75f * geometry().h;
  mainMenuArea.x = geometry().xMid() - 0.5f * mainMenuArea.w;
  mainMenuArea.y = geometry().yMid() - 0.5f * mainMenuArea.h;
  _mainMenu->setGeometry(mainMenuArea);

  _simulation->setGeometry(geometry());

  Size size(geometry().w * 0.75f, geometry().h * 0.75f);
  Point pos(geometry().xMid() - 0.5f * size.w,
            geometry().yMid() - 0.5f * size.h);
  _settingsDialog->setGeometry(Rect(pos, size));

  _mapDialog->setGeometry(Rect(pos, size));

  _gameDialog->setGeometry(Rect(pos, size));
}

void Render::keyboardDownEvent(KeyboardDownEvent *e)
{
  BindingManager *b = BindingManager::instance();
  int keysym = e->event().keysym.sym;
  if (b->findKey("ToggleFPS").check(keysym))
  {
    e->stop();
    _fpsLabel->setVisible(!_fpsLabel->visible());
  }
  else if (b->findKey("Quit").check(keysym))
  {
    e->stop();
    Application::instance()->quit(0);
  }
  else if (b->findKey("Console").check(keysym))
  {
    e->stop();
    if (!_console->visible())
    {
      _consoleSaveEvents.clear();
      WidgetChildren *c = children();
      for (WidgetChildrenIterator it = c->begin();
           it != c->end(); ++it)
      {
        _consoleSaveEvents.push_back((*it)->eventMask());
        if (*it != _console)
          (*it)->disableInput();
      }

      _console->show();
    }
    else
    {
      WidgetChildren *c = children();
      int i = 0;
      for (WidgetChildrenIterator it = c->begin();
           it != c->end(); ++it, ++i)
        (*it)->setEventMask(_consoleSaveEvents[i]);

      _console->hide();
    }
  }
}

void Render::childEvent(Widget *sender, int parameter)
{
  _childEventSender = sender;
  _childEventParameter = parameter;
}

void Render::consoleCommand(const std::string &command)
{
  stringstream s;
  s.str(command);

  string cmd;
  s >> cmd;

  if (cmd == "fps")
  {
    print("FPS counter: on");
    setFPSVisible(true);
  }
  else if (cmd == "nofps")
  {
    print("FPS counter: off");
    setFPSVisible(false);
  }
  else if (cmd == "help")
  {
    print("Available commands:");
    print("  exit, quit");
    print("  fps, nofps");
    print("  sim ...");
  }
  else if ((cmd == "quit") || (cmd == "exit"))
  {
    print("Exit");
    Application::instance()->quit(0);
  }
  else
  {
    print("Invalid command");
  }
}
