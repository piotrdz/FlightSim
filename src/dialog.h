/***************************************************************************
 *   Copyright (C) 2011-2012 by Piotr Dziwinski                            *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* dialog.h
    Contains the base class for dialogs - Dialog */

#pragma once

#include "config.h"

#include "widget.h"
#include "common.h"
#include "label.h"
#include "controls.h"

#include <string>
#include <vector>

class Dialog : public Widget
{
  public:
    Dialog(Widget *pParent,
           const std::string &pName = "");
    virtual ~Dialog();

    virtual void init();
    virtual void render();

    inline std::string title() const
      { return _titleLabel->text(); }

    inline void setTitle(const std::string &pTitle, bool pUTF8)
      { _titleLabel->setText(pTitle);
        _titleLabel->setUTF8Flag(pUTF8); }

    inline Rect clientGeometry() const
      { return _clientGeometry; }

    inline bool focusing() const
      { return _focusing; }

    inline void setFocusing(bool pFocusing)
      { _focusing = pFocusing; }

  private:
    Rect _clientGeometry;
    Label *_titleLabel;
    bool _focusing;
    unsigned int _focusIndex;
    std::vector<Control*> _focusControls;

  protected:
    virtual void resizeEvent();

    virtual void keyboardDownEvent(KeyboardDownEvent *e);
    virtual void mouseMotionEvent(MouseMotionEvent *e);

    virtual void focusControl(Control *control, int focus);
    virtual void addFocusControl(Control *control);
};
