/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* dialog.h
    Zawiera klasę bazową dialogów - Dialog i trzy standardowe dialogi:
    InformationDialog - dialog z tekstem informacyjnym,
    ConfirmationDialog - dialog z pytaniem,
    InputDialog - dialog do pobierania tekstu od użytkownika. */

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
           const std::string &pTitle = "",
           bool pTitleUTF8 = false,
           const std::string &pName = "");
    virtual ~Dialog();

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
