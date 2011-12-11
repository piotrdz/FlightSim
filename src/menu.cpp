/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* menu.cpp
    Zawiera implementację klasy Menu. */

#include "menu.h"

#include "decorator.h"

#include <GL/gl.h>
#include <sstream>

using namespace std;


Menu::Menu(Widget *pParent, const string &title, const vector<string> &items,
           bool pUTF8Flag, const string &pName)
  : Widget(pParent, pName.empty() ? genericName("Menu") : pName)
{
  enableInput();

  _index = 0;
  _titleLabel = new Label(this, title,
                          Decorator::instance()->getFont(FT_MenuTitle),
                          AL_Center, pUTF8Flag,
                          Decorator::instance()->getColor(C_Text),
                          false, name() + "_TitleLabel");
  _titleLabel->show();
  changeItemLabels(items, false);
  setIndex(0);
}

Menu::~Menu()
{
  _titleLabel = NULL;
}

void Menu::setTitle(const string &pTitle)
{
  _titleLabel->setText(pTitle);
  updateGeometry();
}

void Menu::setItems(const vector<string> &items, bool initLabels)
{
  changeItemLabels(items, initLabels);
  setIndex(0);
}

vector<string> Menu::items() const
{
  vector<string> items;
  for (unsigned int i = 0; i < _itemLabels.size(); ++i)
    items.push_back(_itemLabels[i]->text());

  return items;
}

void Menu::setIndex(unsigned int pIndex, int focus)
{
  if (pIndex >= _itemLabels.size())
    return;

  _itemLabels[_index]->setColor(Decorator::instance()->getColor(C_Text));
  if (focus == 1)
  {
    _itemLabels[pIndex]->setColor(Decorator::instance()->getColor(C_HalfFocusedText));
  }
  else
  {
    _itemLabels[pIndex]->setColor(Decorator::instance()->getColor(C_FocusedText));
  }
  _index = pIndex;
}


void Menu::setUTF8Flag(bool pUTF8Flag)
{
  _titleLabel->setUTF8Flag(pUTF8Flag);
  for (unsigned int i = 0; i < _itemLabels.size(); ++i)
    _itemLabels[i]->setUTF8Flag(pUTF8Flag);

}

void Menu::changeItemLabels(const vector<string> &items, bool initLabels)
{
  for (unsigned int i = 0; i < _itemLabels.size(); ++i)
    delete _itemLabels[i];

  _itemLabels.clear();

  for (unsigned int i = 0; i < items.size(); ++i)
  {
    stringstream stream;
    stream << name() << "_Item_" << i;

    Label *label = new Label(this, items[i],
                             Decorator::instance()->getFont(FT_MenuItem),
                             AL_Center, _titleLabel->utf8Flag(),
                             Decorator::instance()->getColor(C_Text),
                             false, stream.str());
    _itemLabels.push_back(label);

    _itemLabels[i]->show();

    if (initLabels)
      _itemLabels[i]->init();
  }

  updateGeometry();
}

void Menu::updateGeometry()
{
  _frame.x = geometry().x;
  _frame.w = geometry().w;

  if (!_titleLabel->text().empty())
  {
    float titleHeight = _titleLabel->textGeometry().h;
    _titleLabel->setGeometry(Rect(geometry().x, geometry().y,
                                  geometry().w, titleHeight));

    _frame.y = geometry().y + titleHeight;
    _frame.h = geometry().h - titleHeight;
  }
  else
  {
    _frame.y = geometry().y;
    _frame.h = geometry().h;
  }

  float labelSize = _frame.h / _itemLabels.size();

  for (unsigned int i = 0; i < _itemLabels.size(); ++i)
  {
    _itemLabels[i]->setGeometry(Rect(_frame.x, _frame.y + i * labelSize,
                                     _frame.w, labelSize));
  }
}

void Menu::render()
{
  Decorator::instance()->renderFrame(_frame);
}

void Menu::resizeEvent()
{
  updateGeometry();
}

void Menu::keyboardDownEvent(KeyboardDownEvent *e)
{
  switch (e->event().keysym.sym)
  {
    case SDLK_UP:
    {
      if (_index > 0)
        setIndex(_index - 1, 1);
      break;
    }
    case SDLK_DOWN:
    {
      if (_index < _itemLabels.size() - 1)
        setIndex(_index + 1, 1);
      break;
    }
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
    case SDLK_SPACE:
    {
      sendChildEvent(ItemChosen);
      break;
    }
    case SDLK_ESCAPE:
    {
      sendChildEvent(EscapePressed);
      break;
    }
    default: {}
  }
}

void Menu::mouseMotionEvent(MouseMotionEvent *e)
{
  if (_itemLabels.empty())
    return;

  Point mousePos(e->event().x, e->event().y);

  if (!_frame.pointInside(mousePos))
  {
    _itemLabels[_index]->setColor(
      Decorator::instance()->getColor(C_HalfFocusedText));
    return;
  }

 for (unsigned int i = 0; i < _itemLabels.size(); ++i)
 {
   if (_itemLabels[i]->geometry().pointInside(mousePos))
   {
     setIndex(i, 2);
     break;
   }
  }
}

void Menu::mouseButtonUpEvent(MouseButtonUpEvent *e)
{
  if (_frame.pointInside(Point(e->event().x, e->event().y)))
    sendChildEvent(ItemChosen);
}
