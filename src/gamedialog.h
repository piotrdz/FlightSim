/***************************************************************************
 *   Copyright (C) 2011-2012 by Piotr Dziwinski                            *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* gamedialog.h
    Contains the GameDialog class - a dialog for setting
    the settings of the game.*/

#pragma once

#include "config.h"

#include "dialog.h"
#include "controls.h"
#include "player.h"

class GameDialog : public Dialog
{
  public:
    enum Actions
    {
      Cancelled,
      Okeyed
    };

  public:
    GameDialog(Widget *pParent);
    virtual ~GameDialog();

    int playerAmmo() const
      { return _playerAmmo; }

    int enemyCount() const
      { return _enemyCount; }

    int enemyActions() const
      { return _enemyActions; }

    virtual void init();

  protected:
    virtual void resizeEvent();

    virtual void keyboardDownEvent(KeyboardDownEvent *e);

    virtual void childEvent(Widget *sender, int parameter);

  private:
    int _playerAmmo;
    int _enemyCount;
    int _enemyActions;

    Label *_playerNameLabel;
    LineEdit *_playerNameEdit;

    Label *_playerAmmoLabel;
    LineEdit *_playerAmmoEdit;

    Label *_enemyCountLabel;
    LineEdit *_enemyCountEdit;

    Label *_enemyActionsLabel;
    CheckBox *_enemyAcceleration;
    CheckBox *_enemyTurning;
    CheckBox *_enemyPitching;
    CheckBox *_enemyEvasiveActions;

    Button *_nextButton, *_backButton;
};
