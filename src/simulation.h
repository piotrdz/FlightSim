/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* simulation.h
    Zawiera klasę Simulation - symulatora lotu. */

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

/** \class Simulation Klasa symulacji
 *
 * Odpowiada za rysowanie widoku z kokpitu samolotu
 */
class Simulation : public Widget
{
  public:
    enum Actions
    {
      Action_Settings,
      Action_EndGame
    };

    //! Jakość wyświetlania = poziom detali mapy
    enum DisplayQuality
    {
      Quality_Low = 0,
      Quality_Medium = 1,
      Quality_High = 2,
      Quality_VeryHigh = 3
    };

    //! Rodzaj widoku
    enum ViewMode
    {
      //! Widok z kokpitu
      View_Cockpit,
      //! Widok z zewnątrz na samolot
      View_Outside
    };

    //! Rodzaj wyświetlanego HUD
    enum HudMode
    {
      //! Brak
      Hud_None,
      //! Tylko wskaźnik kierunku
      Hud_Minimal,
      //! Pełny
      Hud_Full
    };

    //! Rodzaj symulacji
    enum SimulationType
    {
      //! Zwykła symulacja
      Simulation_Normal,
      //! Gra z przeciwnikami
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
    //! Fraktal plazmowy
    Fractal _fractal;
    //! Mapa
    Map *_map;
    //! Czy mapa jest w trakcie wstępnego generowania
    bool _initializing;

    //! Typ symulacji (symulacja, gra z przeciwnikami)
    SimulationType _simulationType;

    //! Gracz
    Player* _player;
    //! Lista graczy - przeciwników
    std::list<Player*> _enemyPlayers;
    //! Czy przeciwnicy zostali wyeliminowani?
    bool _enemiesDestroyed;

    //! Lista pocisków
    std::list<Bullet*> _bullets;

    //! Timer do uaktualniania stanu symulacji
    Timer _updateTimer;

    //! Poziom detali mapy
    DisplayQuality _displayQuality;
    //! Rodzaj widoku
    ViewMode _viewMode;
    //! Szerkość pola widzenia (y)
    float _fov;
    //! Czy widoczna mgła
    bool _fog;
    //! Odległość od samolotu w widoku z zewnątrz
    float _outsideViewZoom;
    //! Kąty (X i Y) widoku z zewnątrz
    Vector3D _outsideViewAngles;
    //! Przyspieszenie zmiany kątów widoku
    Vector3D _outsideViewAnglesAcc;
    //! Rodzaj HUD
    HudMode _hudMode;
    //! @{ Czcionki liczb wyświetlanych na HUD
    Font *_hudFont, *_bigHudFont;
    //! @}
    //! @{ Napisy wyświetlane na HUD
    std::string _heightString, _altitudeString;
    std::string _velocityString, _ammoString;
    //! @}

    //! Menu wyświelane w grze
    Menu *_menu;

    //! Ramka wyświetlana podczas inicjalizacji
    Rect _initializingFrame;
    //! @{ Napisy wyświetlane w czasie gry
    Label *_initializingLabel;
    Label *_collisionLabel;
    Label *_messageLabel;
    //! @}

    //! Timer wiadomości
    Timer _messageTimer;

    //! Zasięg widoku
    static const float VISIBLE_RANGE;
    //! Zasięg radaru
    static const float RADAR_RANGE;

    //! Wyświetla wiadomość na dole ekranu
    void displayMessage(const std::string &message);

    void deleteEnemyPlayers();
    void deleteBullets();
    void resetTimers();
    void renderHud();
};
