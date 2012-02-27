/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* settingsdialog.h
    Contains the SettingsDialog class - a dialog for changing settings. */

#pragma once

#include "config.h"

#include "dialog.h"
#include "controls.h"


class SettingsDialog : public Dialog
{
  public:
    SettingsDialog(Widget *pParent);
    virtual ~SettingsDialog();

  protected:
    virtual void resizeEvent();
    virtual void showEvent();
    virtual void childEvent(Widget *sender, int parameter);


  private:
    Label *_widthLabel, *_heightLabel;
    LineEdit *_widthEdit, *_heightEdit;
    CheckBox *_fullScreenCheckBox;
    CheckBox *_multisamplingCheckBox;
    CheckBox *_fpsCheckBox;
    Label *_displayQualityLabel;
    ChoiceBox *_displayQualityChoiceBox;
    Label *_playerNameLabel;
    LineEdit *_playerNameEdit;
    Label *_fovLabel;
    LineEdit *_fovEdit;
    Label *_noticeLabel;
    Button *_cancelButton, *_okButton;

    void readSettings();
    void writeSettings();
};
