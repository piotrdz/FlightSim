/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* decorator.h
    Contains the Decorator class, which is responsible for drawing interface
    primitives, giving them identical "look'n'feel" (colors, sizes of fonts,
    etc.) and the class RoundRect, drawing rectangles with rounded corners. */

#pragma once

#include "config.h"

#include "common.h"
#include "fontengine.h"

#include <vector>

enum FontType
{
  FT_Small,
  FT_Normal,
  FT_Big,
  FT_MenuTitle,
  FT_MenuItem,
  FT_WindowTitle,
  FT_Button,
  FT_SimulationWarning
};

enum ColorType
{
  C_InterfaceBackground,
  C_SimulationBackground,

  C_Text,
  C_DisabledText,
  C_HalfFocusedText,
  C_FocusedText,

  C_Frame,
  C_WindowFrame,
  C_WindowTitlebar,
  C_ControlFrame,
  C_FocusedControlFrame,
  C_HalfFocusedControlFrame
};

class Widget;

class Decorator : public Object
{
  public:
    Decorator();
    virtual ~Decorator();

    inline static Decorator* instance()
      { return _instance; }

    Font* getFont(FontType type) const;

    Color getColor(ColorType type) const;

    inline float getDefaultMargin() const
      { return _defaultMargin; }

    inline float getDialogMargin() const
      { return _dialogMargin; }

    inline float getControlMargin() const
      { return _controlMargin; }

    void renderFrame(const Rect &frame) const;
    void renderWindowFrame(const Rect &frame, float titlebarHeight) const;

    void renderControlFrame(const Rect &frame, int focus) const;

    void renderButtonFrame(const Rect &frame, int focus) const;
    void renderCheckBoxFrame(const Rect &frame, bool checked, int focus);
    void renderChoiceBoxFrame(const Rect &frame, int focusedBox, int focus);
    void renderEditFrame(const Rect &frame, bool cursor,
                         float cursorX, float cursorHeight,
                         int indicator, int focus) const;

    void windowResized(WindowResizeEvent *e);

    void columnLayout(Point base, const std::vector<Widget*> &widgets,
                      float columnWidth = -1.0f,
                      const std::vector<float> &itemHeights = std::vector<float>());
    void rowLayout(Point base, const std::vector<Widget*> &widgets,
                   float rowHeight = -1.0f,
                   const std::vector<float> &itemWidths = std::vector<float>());
    void gridLayout(Point base, const std::vector< std::vector<Widget*> > &columns,
                    std::vector<float> columnsWidths = std::vector<float>(),
                    std::vector<float> rowsHeights = std::vector<float>());

  private:
    static Decorator* _instance;
    float _defaultMargin, _dialogMargin, _controlMargin;
    float _frameCornerRadius;
    float _buttonCornerRadius;
    float _cursorWidth, _cursorOffset;

    void chooseControlFrameColor(int focus) const;
    void chooseTextColor(int focus) const;
};

class RoundRect
{
  public:
    static void init();

    static void renderSolid(const Rect &rect, float cornerRadius);
    static void renderOutline(const Rect &rect, float cornerRadius);

  private:
    static float _sinValues[45], _cosValues[45];
};
