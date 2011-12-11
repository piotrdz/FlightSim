/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* console.cpp
    Zawiera implementację klasy Console. */

#include "console.h"

#include "decorator.h"

#include <cassert>
#include "console.h"

using namespace std;


Console* Console::_instance = NULL;

Console::Console(Widget* pParent) : Widget(pParent, "Console")
{
  assert(_instance == NULL);
  _instance = this;

  enableInput();

  _commandEditHeight = 0;

  _font = Decorator::instance()->getFont(FT_Small);
  _metrics = new FontMetrics(_font);
  _commandEdit = new LineEdit(this, "", false, _font);
  _commandEdit->show();
  _commandEdit->setFocus(2);
}

Console::~Console()
{
  _instance = NULL;

  delete _metrics;
  _metrics = NULL;

  _font = NULL;
  _commandEdit = NULL;
}

void Console::render()
{
  Decorator::instance()->renderFrame(geometry());

  float margin = Decorator::instance()->getDefaultMargin();
  Point textPosition(geometry().x + margin, geometry().y + margin);

  for (deque<string>::iterator it = _lines.begin(); it != _lines.end(); ++it)
  {
    _font->renderTextUTF8(*it, textPosition);

    textPosition.y += _metrics->height();
  }
}

void Console::print(const std::string& line)
{
  _lines.push_back(line);
  updateLines();
}

void Console::clearCommand()
{
  _commandEdit->setText("");
  _commandEdit->focusIn();
}

void Console::fontResizeEvent(FontResizeEvent *)
{
  updateLines();
}

void Console::resizeEvent()
{
  float margin = Decorator::instance()->getDefaultMargin();

  _commandEditHeight = _commandEdit->preferredSize().h;

  Rect commandEditGeometry;
  commandEditGeometry.x = margin;
  commandEditGeometry.y = geometry().y2() - 2.0f * margin - _commandEditHeight;
  commandEditGeometry.h = _commandEditHeight;
  commandEditGeometry.w = geometry().w - 2.0f * margin;
  _commandEdit->setGeometry(commandEditGeometry);

  updateLines();
}

void Console::updateLines()
{
  float margin = Decorator::instance()->getDefaultMargin();
  unsigned int maxLines = (unsigned int)((size().h - 3.0f * margin - _commandEditHeight)
                                           / _metrics->height());

  while (_lines.size() > maxLines)
    _lines.pop_front();
}

void Console::childEvent(Widget *sender, int parameter)
{
  if ((sender == _commandEdit) && (parameter == LineEdit::ReturnPressed))
  {
    _commandEdit->focusOut();
    sendChildEvent(0);
  }
}
