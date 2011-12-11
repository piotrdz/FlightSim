/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* gamedialog.cpp
    Zawiera implementację klasy GameDialog. */

#include "gamedialog.h"

#include "decorator.h"

#include <sstream>

#include <GL/gl.h>

using namespace std;

GameDialog::GameDialog(Widget* pParent) : Dialog(pParent, "Ustawienia gry")
{
  enableInput();

  _playerAmmo = -1;
  _enemyCount = 1;
  _enemyActions = 0;

  Font *f = Decorator::instance()->getFont(FT_Normal);
  Color c = Decorator::instance()->getColor(C_Text);

  _playerAmmoLabel = new Label(this, "Amunicja (-1 = niesk.):", f,
                               AL_Left | AL_VCenter, true, c);
  _playerAmmoLabel->show();

  _playerAmmoEdit = new LineEdit(this, toString<int>(_playerAmmo));
  _playerAmmoEdit->show();
  addFocusControl(_playerAmmoEdit);

  _enemyCountLabel = new Label(this, "Liczba przeciwników:", f,
                               AL_Left | AL_VCenter, true, c);
  _enemyCountLabel->show();

  _enemyCountEdit = new LineEdit(this, toString<int>(_enemyCount));
  _enemyCountEdit->show();
  addFocusControl(_enemyCountEdit);

  _enemyActionsLabel = new Label(this, "Możliwości AI:", f,
                            AL_Left | AL_VCenter, true, c);
  _enemyActionsLabel->show();

  _enemyAcceleration = new CheckBox(this, "Przyspieszanie", false, true);
  _enemyAcceleration->show();
  addFocusControl(_enemyAcceleration);

  _enemyTurning = new CheckBox(this, "Skręcanie", false, true);
  _enemyTurning->show();
  addFocusControl(_enemyTurning);

  _enemyPitching = new CheckBox(this, "Zmiana wysokości", false, true);
  _enemyPitching->show();
  addFocusControl(_enemyPitching);

  _enemyEvasiveActions = new CheckBox(this, "Manewry unikające", false, true);
  _enemyEvasiveActions->show();
  addFocusControl(_enemyEvasiveActions);

  _nextButton = new Button(this, "Dalej ->", true);
  _nextButton->show();
  addFocusControl(_nextButton);

  _backButton = new Button(this, "<- Powrót", true);
  _backButton->show();
  addFocusControl(_backButton);
}

GameDialog::~GameDialog()
{
}

void GameDialog::resizeEvent()
{
  Dialog::resizeEvent();

  float margin = Decorator::instance()->getControlMargin();

  vector<float> columnsWidth(2);
  columnsWidth[0] = 0.3f * clientGeometry().w;
  columnsWidth[1] = 0.3f * clientGeometry().w;

  vector< vector<Widget*> > columns(2);

  columns[0].push_back(_playerAmmoLabel);
  columns[0].push_back(_enemyCountLabel);
  columns[0].push_back(_enemyActionsLabel);

  columns[1].push_back(_playerAmmoEdit);
  columns[1].push_back(_enemyCountEdit);

  Point base = clientGeometry();

  Decorator::instance()->gridLayout(base, columns, columnsWidth);

  vector<Widget*> actionsColumn;
  actionsColumn.push_back(_enemyAcceleration);
  actionsColumn.push_back(_enemyTurning);
  actionsColumn.push_back(_enemyPitching);
  actionsColumn.push_back(_enemyEvasiveActions);

  Point actionsBase = _enemyActionsLabel->position();
  actionsBase.y += _enemyActionsLabel->geometry().h + margin;

  Decorator::instance()->columnLayout(actionsBase, actionsColumn, 0.6f * clientGeometry().w);


  float buttonsWidth = _backButton->preferredSize().w + _nextButton->preferredSize().w + 2.0f * margin;
  Point buttonsBase = Point(clientGeometry().xMid(), clientGeometry().y2());
  buttonsBase.x -= 0.5f * buttonsWidth;
  buttonsBase.y -= max(_backButton->preferredSize().h, _nextButton->preferredSize().h) + margin;
  _backButton->setGeometry(Rect(buttonsBase, _backButton->preferredSize()));
  buttonsBase.x += _backButton->preferredSize().w + margin;
  _nextButton->setGeometry(Rect(buttonsBase, _nextButton->preferredSize()));
}

void GameDialog::keyboardDownEvent(KeyboardDownEvent* e)
{
  Dialog::keyboardDownEvent(e);

  if (e->event().keysym.sym == SDLK_ESCAPE)
  {
    sendChildEvent(Cancelled);
  }
}

void GameDialog::childEvent(Widget *sender, int parameter)
{
  if (sender == _playerAmmoEdit)
  {
    if ((parameter == LineEdit::ReturnPressed) ||
        (parameter == LineEdit::TextChangedOnFocusOut))
    {
      int a = fromString<int>(_playerAmmoEdit->text());
      if (!((a == -1) || (a > 0)))
        _playerAmmoEdit->setText(toString<int>(_playerAmmo));
      else
        _playerAmmo = a;
    }
  }
  else if (sender == _enemyCountEdit)
  {
    if ((parameter == LineEdit::ReturnPressed) ||
        (parameter == LineEdit::TextChangedOnFocusOut))
    {
      int n = fromString<int>(_enemyCountEdit->text());
      if (n <= 0)
        _enemyCountEdit->setText(toString<int>(_enemyCount));
      else
        _enemyCount = n;
    }
  }
  else if (sender == _enemyAcceleration)
  {
    if (_enemyAcceleration->checked())
      _enemyActions |= Player::AI_Acceleration;
    else
      _enemyActions &= (~Player::AI_Acceleration);
  }
  else if (sender == _enemyTurning)
  {
    if (_enemyTurning->checked())
      _enemyActions |= Player::AI_Turning;
    else
      _enemyActions &= (~Player::AI_Turning);
  }
  else if (sender == _enemyPitching)
  {
    if (_enemyPitching->checked())
      _enemyActions |= Player::AI_Pitching;
    else
      _enemyActions &= (~Player::AI_Pitching);
  }
  else if (sender == _enemyEvasiveActions)
  {
    if (_enemyEvasiveActions->checked())
      _enemyActions |= Player::AI_EvasiveAction;
    else
      _enemyActions &= (~Player::AI_EvasiveAction);
  }
  else if (sender == _backButton)
  {
    sendChildEvent(Cancelled);
  }
  else if (sender == _nextButton)
  {
    sendChildEvent(Okeyed);
  }
}
