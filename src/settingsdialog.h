/***************************************************************************
 *   Copyright (C) 2011-2012 by Piotr Dziwinski                            *
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

    virtual void init();

  protected:
    virtual void resizeEvent();
    virtual void showEvent();
    virtual void childEvent(Widget *sender, int parameter);
    virtual void keyboardDownEvent(KeyboardDownEvent *e);


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
    Label *_languageLabel;
    ChoiceBox *_languageChoiceBox;
    Label *_keyBindingLabel, *_keyBindingChosenLabel;
    ChoiceBox *_keyBindingChoiceBox;
    Button *_keyBindingChangeButton;
    Label *_noticeLabel;
    Button *_cancelButton, *_okButton;
    bool _keyCapture;

    void updateKeyBinding();
    void readSettings();
    void writeSettings();
};
