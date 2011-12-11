/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* menu.h
    Zawiera klasę Menu, która wyświetla listę pozycji do wybrania
    przez użytkownika. */

#pragma once

#include "config.h"

#include "widget.h"
#include "fontengine.h"
#include "label.h"

#include <vector>

class Menu : public Widget
{
  public:
    enum Actions
    {
      ItemChosen,
      EscapePressed
    };

  public:
    Menu(Widget *pParent,
         const std::string &title = "",
         const std::vector<std::string> &items
           = std::vector<std::string>(),
         bool pUTF8Flag = false,
         const std::string &name = "");
    virtual ~Menu();

    std::vector<std::string> items() const;
    void setItems(const std::vector<std::string> &items,
                  bool initLabels = true);

    inline unsigned int index() const
      { return _index; }
    inline void setIndex(unsigned int pIndex, int focus = 2);

    inline std::string title() const
      { return _titleLabel->text(); }
    void setTitle(const std::string &pTitle);

    inline bool utf8Flag() const
      { return _titleLabel->utf8Flag(); }
    void setUTF8Flag(bool pUTF8Flag);

    virtual void render();

  private:
    Label *_titleLabel;
    std::vector<Label*> _itemLabels;
    unsigned int _index;
    Rect _frame;

    void changeItemLabels(const std::vector<std::string> &labels,
                          bool initLabels);
    void updateGeometry();

  protected:
    void resizeEvent();

    virtual void keyboardDownEvent(KeyboardDownEvent *event);
    virtual void mouseMotionEvent(MouseMotionEvent *event);
    virtual void mouseButtonUpEvent(MouseButtonUpEvent *event);
};
