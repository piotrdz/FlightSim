/***************************************************************************
 *   Copyright (C) 2011-2012 by Piotr Dziwinski                            *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

/* decorator.cpp
   Contains the implementation of Decorator and RoundRect classes. */

#include "decorator.h"

#include "filemanager.h"
#include "application.h"
#include "events.h"
#include "widget.h"

#include <cmath>
#include <cassert>

#include <GL/gl.h>

using namespace std;


Decorator* Decorator::_instance = NULL;

Decorator::Decorator() : Object("Decorator")
{
  assert(_instance == NULL);
  _instance = this;

  _defaultMargin = 10.0f;
  _dialogMargin = 15.0f;
  _controlMargin = 12.5f;
  _frameCornerRadius = 20.0f;
  _buttonCornerRadius = 5.0f;
  _cursorWidth = 2.0f;
  _cursorOffset = 0.0f;
}

Decorator::~Decorator()
{
  _instance = NULL;
}

void Decorator::init()
{
  FileManager::instance()->registerFile("InterfaceFont", "data/interface.ttf");
  if (!FileManager::instance()->canRead("InterfaceFont"))
  {
    print(replace(_("Font not found: %1"), "%1", FileManager::instance()->fileName("InterfaceFont")));
    Application::instance()->quit(1);
    return;
  }

  FileManager::instance()->registerFile("InterfaceFontBold", "data/interface_bold.ttf");
  if (!FileManager::instance()->canRead("InterfaceFontBold"))
  {
    print(replace(_("Font not found: %1"), "%1", FileManager::instance()->fileName("InterfaceFontBold")));
    Application::instance()->quit(1);
  }
}

Color Decorator::getColor(ColorType type) const
{
  switch (type)
  {
    case C_InterfaceBackground:
      return Color(0.0f, 0.0f, 0.0f, 0.0f);

    case C_Text:
      return Color(1.0f, 1.0f, 1.0f);

    case C_DisabledText:
      return Color(0.5f, 0.5f, 0.5f);

    case C_HalfFocusedText:
      return Color(0.75f, 0.75f, 0.75f);

    case C_FocusedText:
      return Color(1.0f, 1.0f, 0.2f);

    case C_Frame:
      return Color(0.8f, 0.8f, 0.8f, 0.5f);

    case C_WindowFrame:
      return Color(0.7f, 0.7f, 0.7f, 0.5f);

    case C_WindowTitlebar:
      return Color(0.4f, 0.4f, 0.4f, 0.8f);

    case C_ControlFrame:
      return Color(0.4f, 0.4f, 0.4f, 0.8f);

    case C_HalfFocusedControlFrame:
      return Color(0.5f, 0.5f, 0.5f, 0.8f);

    case C_FocusedControlFrame:
      return Color(0.7f, 0.7f, 0.7f, 0.8f);

    case C_SimulationBackground:
      return Color(0.0f, 0.5f, 0.9f, 0.0f);
  }
  return Color();
}

Font* Decorator::getFont(FontType type) const
{
  switch (type)
  {
    case FT_Small:
     return FontManager::instance()->getFont(
              FileManager::instance()->fileName("InterfaceFont"), 12);

    case FT_Normal:
      return FontManager::instance()->getFont(
               FileManager::instance()->fileName("InterfaceFont"), 16);

    case FT_Big:
      return FontManager::instance()->getFont(
               FileManager::instance()->fileName("InterfaceFont"), 20);

    case FT_MenuTitle:
      return FontManager::instance()->getFont(
               FileManager::instance()->fileName("InterfaceFontBold"), 36);

    case FT_MenuItem:
      return FontManager::instance()->getFont(
               FileManager::instance()->fileName("InterfaceFontBold"), 24);

    case FT_WindowTitle:
      return FontManager::instance()->getFont(
               FileManager::instance()->fileName("InterfaceFontBold"), 20);

    case FT_Button:
      return FontManager::instance()->getFont(
               FileManager::instance()->fileName("InterfaceFontBold"), 16);

    case FT_SimulationWarning:
      return FontManager::instance()->getFont(
               FileManager::instance()->fileName("InterfaceFontBold"), 40);
  }

  return NULL;
}

void Decorator::windowResized(WindowResizeEvent* e)
{
  float multiply = e->newSize().diagonal() / e->previousSize().diagonal();
  _defaultMargin *= multiply;
  _dialogMargin *= multiply;
  _controlMargin *= multiply;
  _frameCornerRadius *= multiply;
  _buttonCornerRadius *= multiply;
}

void Decorator::renderFrame(const Rect &frame) const
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glColor4fv(getColor(C_Frame));

  RoundRect::renderSolid(frame, _frameCornerRadius);

  glDisable(GL_BLEND);
}

void Decorator::renderWindowFrame(const Rect &frame,
                                  float titlebarHeight) const
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glColor4fv(getColor(C_WindowFrame));

  glRectf(frame.x, frame.y, frame.x2(), frame.y2());

  glColor4fv(getColor(C_WindowTitlebar));

  glRectf(frame.x, frame.y, frame.x2(), frame.y + titlebarHeight);

  glDisable(GL_BLEND);
}

void Decorator::chooseControlFrameColor(int focus) const
{
  if (focus == 2)
    glColor4fv(getColor(C_FocusedControlFrame));
  else if (focus == 1)
    glColor4fv(getColor(C_HalfFocusedControlFrame));
  else
    glColor4fv(getColor(C_ControlFrame));
}

void Decorator::chooseTextColor(int focus) const
{
  if (focus == 2)
    glColor4fv(getColor(C_FocusedText));
  else if (focus == 1)
    glColor4fv(getColor(C_HalfFocusedText));
  else
    glColor4fv(getColor(C_Text));
}

void Decorator::renderControlFrame(const Rect &frame, int focus) const
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  chooseControlFrameColor(focus);

  glRectf(frame.x, frame.y, frame.x2(), frame.y2());

  glDisable(GL_BLEND);
}

void Decorator::renderButtonFrame(const Rect &frame, int focus) const
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  chooseControlFrameColor(focus);
  RoundRect::renderSolid(frame, _buttonCornerRadius);

  glDisable(GL_BLEND);
}

void Decorator::renderCheckBoxFrame(const Rect &frame, bool checked,
                                    int focus)
{
  renderControlFrame(frame, focus);

  if (!checked)
    return;

  chooseTextColor(focus);

  float dx = frame.w / 6.0f;
  float dy = frame.h / 6.0f;

  glBegin(GL_QUADS);
  {
    glVertex2f(frame.x2() - 2.0f * dx, frame.y2() - dy);
    glVertex2f(frame.x2() - dx, frame.y2() - dy);
    glVertex2f(frame.x + 2.0f * dx, frame.y + dy);
    glVertex2f(frame.x + dx, frame.y + dy);

    glVertex2f(frame.x + dx, frame.y2() - dy);
    glVertex2f(frame.x + 2.0f * dx, frame.y2() - dy);
    glVertex2f(frame.x2() - dx, frame.y + dy);
    glVertex2f(frame.x2() - 2.0f * dx, frame.y + dy);
  }
  glEnd();
}

void Decorator::renderChoiceBoxFrame(const Rect &frame, int focusedBox,
                                     int focus)
{
  Rect mainFrame = frame;
  mainFrame.w -= 2.0f * frame.h;

  renderControlFrame(mainFrame, focus);

  float d = mainFrame.h / 6.0f;

  Rect box1Frame = frame;
  box1Frame.x = frame.x2() - 2.0f * frame.h;
  box1Frame.w = frame.h;

  int box1Focus = focus;
  if (focusedBox != 1)
  {
    if (focus != 0)
      box1Focus = 1;
    else
      box1Focus = 0;
  }

  renderControlFrame(box1Frame, box1Focus);

  chooseTextColor(box1Focus);

  glBegin(GL_TRIANGLES);
  {
    glVertex2f(box1Frame.x2() - d, box1Frame.y + d);
    glVertex2f(box1Frame.x + d, box1Frame.y + 0.5f * box1Frame.h);
    glVertex2f(box1Frame.x2() - d, box1Frame.y2() - d);
  }
  glEnd();

  Rect box2Frame = frame;
  box2Frame.x = frame.x2() - frame.h;
  box2Frame.w = frame.h;

  int box2Focus = focus;
  if (focusedBox != 2)
  {
    if (focus != 0)
      box2Focus = 1;
    else
      box2Focus = 0;
  }

  renderControlFrame(box2Frame, box2Focus);

  chooseTextColor(box2Focus);

  glBegin(GL_TRIANGLES);
  {
    glVertex2f(box2Frame.x + d, box2Frame.y + d);
    glVertex2f(box2Frame.x + d, box2Frame.y2() - d);
    glVertex2f(box2Frame.x2() - d, box2Frame.y + 0.5f * box2Frame.h);
  }
  glEnd();
}

void Decorator::Decorator::renderEditFrame(const Rect &frame, bool cursor,
       float cursorX, float cursorHeight, int indicator, int focus) const
{
  renderControlFrame(frame, focus);

  glColor4fv(getColor(C_Text));

  if (indicator == 1)
  {
    Rect leftBox = frame;
    leftBox.w = _defaultMargin;
    glBegin(GL_TRIANGLES);
    {
      glVertex2f(leftBox.x2() - leftBox.w / 6.0f, leftBox.y2() - leftBox.h / 6.0f);
      glVertex2f(leftBox.x2() - leftBox.w / 6.0f, leftBox.y + leftBox.h / 6.0f);
      glVertex2f(leftBox.x + leftBox.w / 6.0f, leftBox.y + 0.5f * leftBox.h);
    }
    glEnd();
  }
  else if (indicator == 2)
  {
    Rect rightBox = frame;
    rightBox.x = rightBox.x2() - _defaultMargin;
    rightBox.w = _defaultMargin;
    glBegin(GL_TRIANGLES);
    {
      glVertex2f(rightBox.x + rightBox.w / 6.0f, rightBox.y + rightBox.h / 6.0f);
      glVertex2f(rightBox.x + rightBox.w / 6.0f, rightBox.y2() - rightBox.h / 6.0f);
      glVertex2f(rightBox.x2() - rightBox.w / 6.0f, rightBox.y + 0.5f * rightBox.h);
    }
    glEnd();
  }

  if (cursor)
  {
    glLineWidth(_cursorWidth);

    glBegin(GL_LINES);
    {
      glVertex2f(cursorX + _cursorOffset,
                 frame.y + 0.5f * (frame.h - cursorHeight));
      glVertex2f(cursorX + _cursorOffset,
                 frame.y2() - 0.5f * (frame.h - cursorHeight));
    }
    glEnd();

    glLineWidth(1.0f);
  }
}

void Decorator::columnLayout(Point base, const vector< Widget* >& widgets,
                             float columnWidth, const vector<float> &itemHeights)
{
  if (columnWidth < 0.0f)
  {
    for (unsigned int i = 0; i < widgets.size(); ++i)
    {
      if (widgets[i]->preferredSize().w > columnWidth)
        columnWidth = widgets[i]->preferredSize().w;
    }
  }

  for (unsigned int i = 0; i < widgets.size(); ++i)
  {
    float h = widgets[i]->preferredSize().h;
    if ((i < itemHeights.size()) && (itemHeights[i] >= 0.0f))
      h = itemHeights[i];

    widgets[i]->setGeometry(Rect(base, Size(columnWidth, h)));
    base.y += h + getControlMargin();
  }
}

void Decorator::rowLayout(Point base, const vector< Widget* >& widgets,
                          float rowHeight, const vector<float> &itemWidths)
{
  if (rowHeight < 0.0f)
  {
    for (unsigned int i = 0; i < widgets.size(); ++i)
    {
      if (widgets[i]->preferredSize().h > rowHeight)
        rowHeight = widgets[i]->preferredSize().h;
    }
  }

  for (unsigned int i = 0; i < widgets.size(); ++i)
  {
    float w = widgets[i]->preferredSize().w;
    if ((i < itemWidths.size()) && (itemWidths[i] >= 0.0f))
      w = itemWidths[i];

    widgets[i]->setGeometry(Rect(base, Size(w, rowHeight)));
    base.x += w + getControlMargin();
  }
}

void Decorator::gridLayout(Point base,
                           const vector< vector<Widget*> > &columns,
                           vector<float> columnsWidths,
                           vector<float> rowsHeights)
{
  unsigned int colCount = columns.size();
  unsigned int rowCount = 0;
  for (unsigned int col = 0; col < colCount; ++col)
  {
    if (columns[col].size() > rowCount)
      rowCount = columns[col].size();
  }

  for (unsigned int col = 0; col < colCount; ++col)
  {
    if (col >= columnsWidths.size())
      columnsWidths.push_back(-1);

    if (columnsWidths[col] == -1)
    {
      for (unsigned int row = 0; row < rowCount; ++row)
      {
        if (row < columns[col].size())
        {
          if (columns[col][row]->preferredSize().w > columnsWidths[col])
            columnsWidths[col] = columns[col][row]->preferredSize().w;
        }
      }
    }
  }

  for (unsigned int row = 0; row < rowCount; ++row)
  {
    if (row >= rowsHeights.size())
      rowsHeights.push_back(-1);

    if (rowsHeights[row] == -1)
    {
      for (unsigned int col = 0; col < colCount; ++col)
      {
        if (row < columns[col].size())
        {
          if (columns[col][row]->preferredSize().h > rowsHeights[row])
            rowsHeights[row] = columns[col][row]->preferredSize().h;
        }
      }
    }
  }

  Point location = base;
  for (unsigned int col = 0; col < colCount; ++col)
  {
    location.y = base.y;
    for (unsigned int row = 0; row < rowCount; ++row)
    {
      if (row < columns[col].size())
      {
        columns[col][row]->setGeometry(Rect(location,
          Size(columnsWidths[col], rowsHeights[row])));
        location.y += rowsHeights[row];
        location.y += getControlMargin();
      }
    }
    location.x += columnsWidths[col];
    location.x += getControlMargin();
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

float RoundRect::_sinValues[] = { 0.0f };
float RoundRect::_cosValues[] = { 0.0f };

void RoundRect::init()
{
  _sinValues[0] = 0.0f;
  _cosValues[0] = 1.0f;
  _sinValues[44] = 1.0f;
  _cosValues[44] = 0.0f;

  for (int i = 1; i < 44; ++i)
  {
    _sinValues[i] = sin(i * M_PI / 90.0f);
    _cosValues[i] = cos(i * M_PI / 90.0f);
  }
}

void RoundRect::renderSolid(const Rect &rect, float cornerRadius)
{
  if (_cosValues[0] == 0.0f)
    init();

  glRectf(rect.x, rect.y + cornerRadius,
          rect.x + rect.w, rect.y + rect.h - cornerRadius);
  glRectf(rect.x + cornerRadius, rect.y,
          rect.x + rect.w - cornerRadius, rect.y + cornerRadius);
  glRectf(rect.x + cornerRadius, rect.y + rect.h - cornerRadius,
          rect.x + rect.w - cornerRadius, rect.y + rect.h);

  glPushMatrix();
  {
    glTranslatef(rect.x + cornerRadius, rect.y + cornerRadius, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
    {
      glVertex2f(0.0f, 0.0f);

      for (int i = 0; i < 45; ++i)
      {
        glVertex2f(-cornerRadius * _cosValues[i],
                   -cornerRadius * _sinValues[i]);
      }
    }
    glEnd();

    glTranslatef(rect.w - 2.0f * cornerRadius, 0.0f, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
    {
      glVertex2f(0.0f, 0.0f);
      for (int i = 0; i < 45; ++i)
      {
        glVertex2f( cornerRadius * _sinValues[i],
                   -cornerRadius * _cosValues[i]);
      }
    }
    glEnd();

    glTranslatef(0.0f, rect.h - 2.0f * cornerRadius, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
    {
      glVertex2f(0.0f, 0.0f);
      for (int i = 0; i < 45; ++i)
      {
        glVertex2f(cornerRadius * _cosValues[i],
                   cornerRadius * _sinValues[i]);
      }
    }
    glEnd();

    glTranslatef(-rect.w + 2.0f * cornerRadius, 0.0f, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
    {
      glVertex2f(0.0f, 0.0f);
      for (int i = 0; i < 45; ++i)
      {
        glVertex2f(-cornerRadius * _sinValues[i],
                    cornerRadius * _cosValues[i]);
      }
    }
    glEnd();
  }
  glPopMatrix();
}

void RoundRect::renderOutline(const Rect &rect, float cornerRadius)
{
  if (_cosValues[0] == 0.0f)
    init();

  float w = rect.w - 2.0f * cornerRadius;
  float h = rect.h - 2.0f * cornerRadius;

  glPushMatrix();
  {
    glTranslatef(rect.x + cornerRadius, rect.y + cornerRadius, 0.0f);

    glBegin(GL_LINE_LOOP);
    {
      for (int i = 0; i < 45; ++i)
      {
        glVertex2f(-cornerRadius * _cosValues[i],
                   -cornerRadius * _sinValues[i]);
      }

      glVertex2f(w, -cornerRadius);

      for (int i = 0; i < 45; ++i)
      {
        glVertex2f(w + cornerRadius * _sinValues[i],
                   -cornerRadius * _cosValues[i]);
      }

      glVertex2f(w + cornerRadius, h);

      for (int i = 0; i < 45; ++i)
      {
        glVertex2f(w + cornerRadius * _cosValues[i],
                  h + cornerRadius * _sinValues[i]);
      }

      glVertex2f(0.0f, h + cornerRadius);

      for (int i = 0; i < 45; ++i)
      {
        glVertex2f(-cornerRadius * _sinValues[i],
                   h + cornerRadius * _cosValues[i]);
      }

      glVertex2f(-cornerRadius, 0.0f);
    }
    glEnd();
  }
  glPopMatrix();
}
