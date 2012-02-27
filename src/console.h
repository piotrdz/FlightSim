/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* console.h
    Contains the Console class, which displays an in-program console with
    the a command prompt. */

#pragma once

#include "config.h"

#include "widget.h"
#include "fontengine.h"
#include "controls.h"

#include <deque>
#include <string>

class Console : public Widget
{
  public:
    Console(Widget *pParent);
    virtual ~Console();

    inline static Console *instance()
      { return _instance; }

    virtual void render();

    void print(const std::string &line);

    inline std::string command() const
      { return _commandEdit->text(); }

    void clearCommand();

  protected:
    virtual void resizeEvent();
    virtual void fontResizeEvent(FontResizeEvent *e);

    virtual void childEvent(Widget *sender, int parameter);

  private:
    static Console *_instance;
    Font *_font;
    FontMetrics *_metrics;
    std::deque<std::string> _lines;
    float _commandEditHeight;
    LineEdit *_commandEdit;

    void updateLines();
};
