/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

  /* controls.h
     Zawiera klasę kontrolki Control i klasy pochodne:
     Button - przycisk,
     CheckBox - pole do zaznaczania,
     ChoiceBox - pole do wyboru opcji,
     LineEdit - pole do wpisywania tekstu */

#pragma once

#include "config.h"

#include "widget.h"
#include "label.h"

#include <vector>
#include <string>

class Control : public Widget
{
  public:
    Control(Widget *pParent, const std::string &pName = "");
    virtual ~Control();

    inline int focus() const
      { return _focus; }
    void setFocus(int pFocus);

    virtual void focusIn() {}
    virtual void focusOut() {}

  protected:
    int _focus;
};


class Button : public Control
{
  public:
    enum Actions
    {
      Clicked
    };

  public:
    Button(Widget *pParent,
           const std::string &pText = "",
           bool pUTF8Flag = false,
           Font *pCustomFont = NULL,
           const std::string &pName = "");
    virtual ~Button();

    inline std::string text() const
      { return _label->text(); }
    inline void setText(const std::string &text)
      { _label->setText(text); }

    inline bool utf8Flag() const
      { return _label->utf8Flag(); }
    void setUTF8Flag(bool pUTF8Flag)
      { _label->setUTF8Flag(pUTF8Flag); }

    virtual void render();

    virtual Size preferredSize() const;

    virtual void focusIn();
    virtual void focusOut();

  private:
    Label *_label;

  protected:
    virtual void resizeEvent();

    virtual void keyboardDownEvent(KeyboardDownEvent *e);
    virtual void mouseButtonUpEvent(MouseButtonUpEvent *e);
};

class CheckBox : public Control
{
  public:
    enum Actions
    {
      Checked,
      Unchecked
    };

  public:
    CheckBox(Widget *pParent,
             const std::string &pText = "",
             bool pChecked = false,
             bool pUTF8Flag = false,
             Font *pCustomFont = NULL,
             const std::string &pName = "");
    virtual ~CheckBox();

    inline std::string text() const
      { return _label->text(); }
    inline void setText(const std::string &text)
      { _label->setText(text); }

    inline bool utf8Flag() const
      { return _label->utf8Flag(); }
    void setUTF8Flag(bool pUTF8Flag)
      { _label->setUTF8Flag(pUTF8Flag); }

    inline bool checked() const
      { return _checked; }
    void setChecked(bool pChecked)
      { _checked = pChecked; }

    virtual void render();

    virtual Size preferredSize() const;

    virtual void focusIn();
    virtual void focusOut();

  private:
    Label *_label;
    Rect _boxFrame;
    bool _checked;

  protected:
    virtual void resizeEvent();

    virtual void keyboardDownEvent(KeyboardDownEvent *e);
    virtual void mouseButtonUpEvent(MouseButtonUpEvent *e);
};

class ChoiceBox : public Control
{
  public:
    enum Actions
    {
      ChoiceChanged
    };

  public:
    ChoiceBox(Widget *pParent,
              const std::vector<std::string> &pChoices
                = std::vector<std::string>(),
              unsigned int pIndex = 0,
              bool pUTF8Flag = false,
              Font *pCustomFont = NULL,
              const std::string &pName = "");
    virtual ~ChoiceBox();

    std::vector<std::string> choices()
      { return _choices; }
    void setChoices(std::vector<std::string> &pChoices);

    inline bool utf8Flag() const
      { return _label->utf8Flag(); }
    void setUTF8Flag(bool pUTF8Flag)
      { _label->setUTF8Flag(pUTF8Flag); }

    inline unsigned int index() const
      { return _index; }
    void setIndex(unsigned int pIndex);

    virtual void render();

    virtual Size preferredSize() const
      { return _preferredSize; }

    virtual void focusIn();
    virtual void focusOut();

  private:
    Label *_label;
    std::vector<std::string> _choices;
    int _index;
    Size _preferredSize;
    Rect _box1Rect, _box2Rect;
    int _focusedBox;

    void updatePreferredSize();

  protected:
    virtual void resizeEvent();
    virtual void keyboardDownEvent(KeyboardDownEvent *e);
    virtual void mouseMotionEvent(MouseMotionEvent *e);
    virtual void mouseButtonUpEvent(MouseButtonUpEvent *e);
};

class LineEdit : public Control
{
  public:
    enum Actions
    {
      TextChanged,
      TextChangedOnFocusOut,
      ReturnPressed
    };
  public:
    LineEdit(Widget *pParent,
             const std::string &pText = "",
             bool pUTF8Flag = false,
             Font *pCustomFont = NULL,
             const std::string &pName = "");
    virtual ~LineEdit();

    std::string text() const;
    void setText(const std::string &pText);

    inline bool utf8Flag() const
      { return _label->utf8Flag(); }
    void setUTF8Flag(bool pUTF8Flag)
      { _label->setUTF8Flag(pUTF8Flag); }

    virtual void render();
    virtual void update();

    virtual Size preferredSize() const;
    virtual void focusIn();
    virtual void focusOut();

  private:
    std::wstring _text, _previousText;
    unsigned int _textOffset;
    int _indicator;
    Label *_label;
    unsigned int _cursorPos;
    float _cursorX, _cursorHeight;
    bool _cursorVisible;
    Timer _blinkTimer;

    void updateLabel();

  protected:
    void resizeEvent();

    void keyboardDownEvent(KeyboardDownEvent *e);
    void mouseButtonUpEvent(MouseButtonUpEvent *e);
};
