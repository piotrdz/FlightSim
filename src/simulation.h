/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* simulation.h
    Contains the Simulation class - the flight simulator. */

#pragma once

#include "config.h"

#include "common.h"
#include "widget.h"
#include "map.h"
#include "fractal.h"
#include "bullet.h"
#include "player.h"

#include <list>

class Label;
class Font;
class Menu;

class Simulation : public Widget
{
  public:
    enum Actions
    {
      Action_Settings,
      Action_EndGame
    };

    enum DisplayQuality
    {
      Quality_Low = 0,
      Quality_Medium = 1,
      Quality_High = 2,
      Quality_VeryHigh = 3
    };

    enum ViewMode
    {
      View_Cockpit,
      View_Outside
    };

    enum HudMode
    {
      Hud_None,
      Hud_Minimal,
      Hud_Full
    };

    enum SimulationType
    {
      Simulation_Normal,
      Simulation_Game
    };

  public:
    Simulation(Widget* pParent,
               const std::string &pName);
    virtual ~Simulation();

    inline void setFractalOptions(const FractalOptions &pOptions)
      { _fractal.setOptions(pOptions); }

    inline void setScale(const Vector3D &pScale)
      { _map->setScale(pScale); }

    inline void setType(SimulationType pType)
      { _simulationType = pType; }
    inline SimulationType type() const
      { return _simulationType; }

    inline void setPlayerAmmo(int ammo)
      { _player->setAmmo(ammo); }

    void loadSettings();
    void reset();

    void addEnemies(int count, int aiActions);

    virtual void init();
    virtual void render();
    virtual void update();

    void settingsDialogFinished();

    void command(const std::string &commandStr);

  protected:
    virtual void resizeEvent();
    virtual void showEvent();
    virtual void hideEvent();

    virtual void keyboardDownEvent(KeyboardDownEvent* e);
    virtual void keyboardUpEvent(KeyboardUpEvent* e);

    virtual void joystickAxisMotionEvent(JoystickAxisMotionEvent* e);
    virtual void joystickButtonDownEvent(JoystickButtonDownEvent* e);
    virtual void joystickButtonUpEvent(JoystickButtonUpEvent* e);
    virtual void joystickHatEvent(JoystickHatEvent* e);

    virtual void childEvent(Widget *sender, int parameter);

  private:
    Fractal _fractal;
    Map *_map;
    bool _initializing;

    SimulationType _simulationType;
    Player* _player;
    std::list<Player*> _enemyPlayers;
    bool _enemiesDestroyed;
    std::list<Bullet*> _bullets;
    Timer _updateTimer;

    DisplayQuality _displayQuality;
    ViewMode _viewMode;
    float _fov;
    bool _fog;
    float _outsideViewZoom;
    Vector3D _outsideViewAngles;
    Vector3D _outsideViewAnglesAcc;

    HudMode _hudMode;

    Font *_hudFont, *_bigHudFont;

    std::string _heightString, _altitudeString;
    std::string _velocityString, _ammoString;

    Menu *_menu;

    Rect _initializingFrame;

    Label *_initializingLabel;
    Label *_collisionLabel;
    Label *_messageLabel;

    Timer _messageTimer;

    static const float VISIBLE_RANGE;
    static const float RADAR_RANGE;

    void displayMessage(const std::string &message);
    void deleteEnemyPlayers();
    void deleteBullets();
    void resetTimers();
    void renderHud();
};
