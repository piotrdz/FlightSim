/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* settingsdialog.cpp
    Zawiera implementację klasy SettingsDialog. */

#include "settingsdialog.h"

#include "decorator.h"
#include "settings.h"

using namespace std;


SettingsDialog::SettingsDialog(Widget *pParent) : Dialog(pParent, "Ustawienia")
{
  enableInput();

  Font *f = Decorator::instance()->getFont(FT_Normal);
  Color c = Decorator::instance()->getColor(C_Text);

  _widthLabel = new Label(this, "Szerokość ekranu*:", f,
                          AL_Left | AL_VCenter, true, c);
  _widthLabel->show();

  _widthEdit = new LineEdit(this);
  _widthEdit->show();
  addFocusControl(_widthEdit);

  _heightLabel = new Label(this, "Wysokość ekranu*:", f,
                          AL_Left | AL_VCenter, true, c);
  _heightLabel->show();

  _heightEdit = new LineEdit(this);
  _heightEdit->show();
  addFocusControl(_heightEdit);

  _fullScreenCheckBox = new CheckBox(this, "Pełny ekran*", false, true);
  _fullScreenCheckBox->show();
  addFocusControl(_fullScreenCheckBox);

  _multisamplingCheckBox = new CheckBox(this, "Multisampling*", false, true);
  _multisamplingCheckBox->show();
  addFocusControl(_multisamplingCheckBox);

  _fpsCheckBox = new CheckBox(this, "Licznik FPS", false, true);
  _fpsCheckBox->show();
  addFocusControl(_fpsCheckBox);

  _displayQualityLabel = new Label(this, "Poziom detali:", f,
                                   AL_Left | AL_VCenter, true, c);
  _displayQualityLabel->show();

  vector<string> displayQualityChoices;
  displayQualityChoices.push_back("Niski");
  displayQualityChoices.push_back("Średni");
  displayQualityChoices.push_back("Wysoki");
  displayQualityChoices.push_back("Bardzo wysoki");
  _displayQualityChoiceBox = new ChoiceBox(this, displayQualityChoices, 0, true);
  _displayQualityChoiceBox->show();
  addFocusControl(_displayQualityChoiceBox);

  _playerNameLabel = new Label(this, "Nazwa gracza:", f,
                               AL_Left | AL_VCenter, true, c);
  _playerNameLabel->show();

  _playerNameEdit = new LineEdit(this);
  _playerNameEdit->show();
  addFocusControl(_playerNameEdit);

  _fovLabel = new Label(this, "Kąt widoku [°]:", f,
                        AL_Left | AL_VCenter, true, c);
  _fovLabel->show();

  _fovEdit = new LineEdit(this);
  _fovEdit->show();
  addFocusControl(_fovEdit);

  _noticeLabel = new Label(this, "* - zastosowane po ponownym uruchomieniu", f,
                           AL_Left | AL_VCenter, true, c);
  _noticeLabel->show();

  _okButton = new Button(this, "OK", true);
  _okButton->show();
  addFocusControl(_okButton);

  _cancelButton = new Button(this, "Anuluj", true);
  _cancelButton->show();
  addFocusControl(_cancelButton);
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::resizeEvent()
{
  Dialog::resizeEvent();

  float margin = Decorator::instance()->getControlMargin();

  vector<float> columnsWidths(2);
  columnsWidths[0] = 0.25f * clientGeometry().w;
  columnsWidths[1] = 0.3f * clientGeometry().w;

  vector< vector<Widget*> > columns1(2);

  columns1[0].push_back(_widthLabel);
  columns1[0].push_back(_heightLabel);

  columns1[1].push_back(_widthEdit);
  columns1[1].push_back(_heightEdit);

  Point columns1Base = clientGeometry();

  Decorator::instance()->gridLayout(columns1Base, columns1, columnsWidths);

  vector<Widget*> checkBoxColumn;
  checkBoxColumn.push_back(_fullScreenCheckBox);
  checkBoxColumn.push_back(_multisamplingCheckBox);
  checkBoxColumn.push_back(_fpsCheckBox);

  Point checkBoxColumnBase = Point(_heightLabel->geometry().x, _heightLabel->geometry().y2());
  checkBoxColumnBase.y += margin;

  Decorator::instance()->columnLayout(checkBoxColumnBase, checkBoxColumn, -1);

  vector< vector<Widget*> > columns2(2);

  columns2[0].push_back(_displayQualityLabel);
  columns2[0].push_back(_playerNameLabel);
  columns2[0].push_back(_fovLabel);

  columns2[1].push_back(_displayQualityChoiceBox);
  columns2[1].push_back(_playerNameEdit);
  columns2[1].push_back(_fovEdit);

  Point columns2Base = Point(_fpsCheckBox->geometry().x, _fpsCheckBox->geometry().y2());
  columns2Base.y += margin;

  Decorator::instance()->gridLayout(columns2Base, columns2, columnsWidths);


  float buttonsWidth = _okButton->preferredSize().w + _cancelButton->preferredSize().w + 2.0f * margin;
  Point buttonsBase = Point(clientGeometry().xMid(), clientGeometry().y2());
  buttonsBase.x -= 0.5f * buttonsWidth;
  buttonsBase.y -= max(_okButton->preferredSize().h, _cancelButton->preferredSize().h) + margin;
  _okButton->setGeometry(Rect(buttonsBase, _okButton->preferredSize()));
  buttonsBase.x += _okButton->preferredSize().w + margin;
  _cancelButton->setGeometry(Rect(buttonsBase, _cancelButton->preferredSize()));

  Point noticePos;
  noticePos.y = _okButton->geometry().y - _noticeLabel->preferredSize().h -
                     margin;
  noticePos.x = clientGeometry().x;
  _noticeLabel->setGeometry(Rect(noticePos, _noticeLabel->preferredSize()));
}

void SettingsDialog::showEvent()
{
  readSettings();
}

void SettingsDialog::childEvent(Widget *sender, int parameter)
{
  if (sender == _cancelButton)
  {
    sendChildEvent(0);
  }
  else if (sender == _okButton)
  {
    writeSettings();
    sendChildEvent(0);
  }
  else if (sender == _widthEdit)
  {
    if ((parameter == LineEdit::ReturnPressed) ||
        (parameter == LineEdit::TextChangedOnFocusOut))
    {
      int w = fromString<int>(_widthEdit->text());
      if (w <= 0)
        w = 1024;
      _widthEdit->setText(toString<int>(w));
    }
  }
  else if (sender == _heightEdit)
  {
    if ((parameter == LineEdit::ReturnPressed) ||
        (parameter == LineEdit::TextChangedOnFocusOut))
    {
      int h = fromString<int>(_heightEdit->text());
      if (h <= 0)
        h = 768;
      _heightEdit->setText(toString<int>(h));
    }
  }
  else if (sender == _fovEdit)
  {
    if ((parameter == LineEdit::ReturnPressed) ||
        (parameter == LineEdit::TextChangedOnFocusOut))
    {
      float f = fromString<float>(_fovEdit->text());
      if ((f <= 0) || (f >= 180.0f))
        f = 45.0f;
      _fovEdit->setText(toString<float>(f));
    }
  }
}

void SettingsDialog::readSettings()
{
  Settings *s = Settings::instance();
  _widthEdit->setText(s->setting<string>("ResolutionX"));
  _heightEdit->setText(s->setting<string>("ResolutionY"));
  _fullScreenCheckBox->setChecked(s->setting<bool>("Fullscreen"));
  _multisamplingCheckBox->setChecked(s->setting<bool>("Multisampling"));
  _fpsCheckBox->setChecked(s->setting<bool>("FPS"));
  _displayQualityChoiceBox->setIndex(s->setting<int>("DisplayQuality"));
  _playerNameEdit->setText(s->setting<string>("PlayerName"));
  _fovEdit->setText(s->setting<string>("FOV"));
}

void SettingsDialog::writeSettings()
{
  Settings *s = Settings::instance();
  s->setSetting<int>("ResolutionX", fromString<int>(_widthEdit->text()));
  s->setSetting<int>("ResolutionY", fromString<int>(_heightEdit->text()));
  s->setSetting<bool>("Fullscreen", _fullScreenCheckBox->checked());
  s->setSetting<bool>("Multisampling", _multisamplingCheckBox->checked());
  s->setSetting<bool>("FPS", _fpsCheckBox->checked());
  s->setSetting<int>("DisplayQuality", _displayQualityChoiceBox->index());
  s->setSetting<string>("PlayerName", _playerNameEdit->text());
  s->setSetting<float>("FOV", fromString<float>(_fovEdit->text()));
}
