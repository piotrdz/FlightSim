/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* mapdialog.cpp
    Contains the implementation of the MapDialog class. */

#include "mapdialog.h"

#include "decorator.h"

#include <sstream>

#include <GL/gl.h>

using namespace std;

MapDialog::MapDialog(Widget* pParent) : Dialog(pParent, "Map settings")
{
  enableInput();

  _hasTexture = false;

  _scale = Vector3D(20.0f, 800.0f, 20.0f);

  Font *f = Decorator::instance()->getFont(FT_Normal);
  Color c = Decorator::instance()->getColor(C_Text);

  _distributionLabel = new Label(this, "Random distribution:", f,
                                 AL_Left | AL_VCenter, true, c);
  _distributionLabel->show();

  vector<string> distributionChoices;
  distributionChoices.push_back("Uniform");
  distributionChoices.push_back("Normal");
  distributionChoices.push_back("Weibull");
  _distributionChoiceBox = new ChoiceBox(this, distributionChoices, 1, true);
  _distributionChoiceBox->show();
  addFocusControl(_distributionChoiceBox);

  _param1Label = new Label(this, "Mean:", f,
                           AL_Left | AL_VCenter, true, c);
  _param1Label->show();
  _param1Edit = new LineEdit(this);
  _param1Edit->show();
  addFocusControl(_param1Edit);

  _param2Label = new Label(this, "Std deviation:", f,
                           AL_Left | AL_VCenter, true, c);
  _param2Label->show();
  _param2Edit = new LineEdit(this);
  _param2Edit->show();
  addFocusControl(_param2Edit);


  _clampingLabel = new Label(this, "Clamping:", f,
                             AL_Left | AL_VCenter, true, c);
  _clampingLabel->show();

  vector<string> clampingChoices;
  clampingChoices.push_back("Rounding");
  clampingChoices.push_back("Repetitive generation");
  _clampingChoiceBox = new ChoiceBox(this, clampingChoices, 0, true);
  _clampingChoiceBox->show();
  addFocusControl(_clampingChoiceBox);

  _minLabel = new Label(this, "Min. range:", f,
                        AL_Left | AL_VCenter, true, c);
  _minLabel->show();
  _minEdit = new LineEdit(this);
  _minEdit->show();
  addFocusControl(_minEdit);

  _maxLabel = new Label(this, "Max. range:", f,
                        AL_Left | AL_VCenter, true, c);
  _maxLabel->show();
  _maxEdit = new LineEdit(this);
  _maxEdit->show();
  addFocusControl(_maxEdit);


  _mixingLabel = new Label(this, "Mixing function:", f,
                           AL_Left | AL_VCenter, true, c);
  _mixingLabel->show();

  vector<string> mixingChoices;
  mixingChoices.push_back("Gauss");
  mixingChoices.push_back("Linear");
  _mixingChoiceBox = new ChoiceBox(this, mixingChoices, 1, true);
  _mixingChoiceBox->show();
  addFocusControl(_mixingChoiceBox);

  _mixingTwopointLabel = new Label(this, "Coeff. for 2-pt f.:",
                                 f, AL_Left | AL_VCenter, true, c);
  _mixingTwopointLabel->show();
  _mixingTwopointEdit = new LineEdit(this);
  _mixingTwopointEdit->show();
  addFocusControl(_mixingTwopointEdit);

  _mixingFourpointLabel = new Label(this, "Coeff. for 4-pt f.:",
                                    f, AL_Left | AL_VCenter, true, c);
  _mixingFourpointLabel->show();

  _mixingFourpointEdit = new LineEdit(this);
  _mixingFourpointEdit->show();
  addFocusControl(_mixingFourpointEdit);


  _scaleLabel = new Label(this, "Map scale:", f,
                          AL_Left | AL_VCenter, true, c);
  _scaleLabel->show();

  _xLabel = new Label(this, "X:", f, AL_Left | AL_VCenter, true, c);
  _xLabel->show();
  _xEdit = new LineEdit(this);
  _xEdit->show();
  addFocusControl(_xEdit);

  _yLabel = new Label(this, "Y:", f, AL_Left | AL_VCenter, true, c);
  _yLabel->show();
  _yEdit = new LineEdit(this);
  _yEdit->show();
  addFocusControl(_yEdit);

  _zLabel = new Label(this, "Z:", f, AL_Left | AL_VCenter, true, c);
  _zLabel->show();
  _zEdit = new LineEdit(this);
  _zEdit->show();
  addFocusControl(_zEdit);

  _nextButton = new Button(this, "Next ->", true);
  _nextButton->show();
  addFocusControl(_nextButton);

  _backButton = new Button(this, "<- Back", true);
  _backButton->show();
  addFocusControl(_backButton);

  _previewLabel = new Label(this, "Preview:", f, AL_Left | AL_VCenter, true, c);
  _previewLabel->show();

  writeControls();
}

MapDialog::~MapDialog()
{
}

void MapDialog::init()
{
  Dialog::init();

  generateTexture();
}

void MapDialog::render()
{
  Dialog::render();

  if (!_hasTexture)
    return;

  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, _texture);

  glColor3f(1.0f, 1.0f, 1.0f);

  glBegin(GL_QUADS);
  {
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(_textureFrame.x, _textureFrame.y2());

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(_textureFrame.x2(), _textureFrame.y2());

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(_textureFrame.x2(), _textureFrame.y);

    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(_textureFrame.x, _textureFrame.y);
  }
  glEnd();

  glDisable(GL_TEXTURE_2D);
}

void MapDialog::resizeEvent()
{
  Dialog::resizeEvent();

  float margin = Decorator::instance()->getControlMargin();

  vector<float> columnsWidth(2);
  columnsWidth[0] = 0.25f * clientGeometry().w;
  columnsWidth[1] = 0.3f * clientGeometry().w;

  vector< vector<Widget*> > columns(2);

  columns[0].push_back(_distributionLabel);
  columns[0].push_back(_param1Label);
  columns[0].push_back(_param2Label);
  columns[0].push_back(_clampingLabel);
  columns[0].push_back(_minLabel);
  columns[0].push_back(_maxLabel);
  columns[0].push_back(_mixingLabel);
  columns[0].push_back(_mixingTwopointLabel);
  columns[0].push_back(_mixingFourpointLabel);
  columns[0].push_back(_scaleLabel);

  columns[1].push_back(_distributionChoiceBox);
  columns[1].push_back(_param1Edit);
  columns[1].push_back(_param2Edit);
  columns[1].push_back(_clampingChoiceBox);
  columns[1].push_back(_minEdit);
  columns[1].push_back(_maxEdit);
  columns[1].push_back(_mixingChoiceBox);
  columns[1].push_back(_mixingTwopointEdit);
  columns[1].push_back(_mixingFourpointEdit);

  Point base = clientGeometry();

  Decorator::instance()->gridLayout(base, columns, columnsWidth);

  vector<Widget*> scaleRow;
  scaleRow.push_back(_xLabel);
  scaleRow.push_back(_xEdit);
  scaleRow.push_back(_yLabel);
  scaleRow.push_back(_yEdit);
  scaleRow.push_back(_zLabel);
  scaleRow.push_back(_zEdit);

  vector<float> scaleRowWidths;
  scaleRowWidths.push_back(-1);
  scaleRowWidths.push_back(0.1f * clientGeometry().w);
  scaleRowWidths.push_back(-1);
  scaleRowWidths.push_back(0.1f * clientGeometry().w);
  scaleRowWidths.push_back(-1);
  scaleRowWidths.push_back(0.1f * clientGeometry().w);

  Point scaleRowBase = Point(_scaleLabel->geometry().x, _scaleLabel->geometry().y2());
  scaleRowBase.y += margin;

  Decorator::instance()->rowLayout(scaleRowBase, scaleRow, -1, scaleRowWidths);

  float buttonsWidth = _backButton->preferredSize().w + _nextButton->preferredSize().w +
                       2.0f * margin;
  Point buttonsBase = Point(clientGeometry().xMid(), clientGeometry().y2());
  buttonsBase.x -= 0.5f * buttonsWidth;
  buttonsBase.y -= max(_backButton->preferredSize().h, _nextButton->preferredSize().h) + margin;
  _backButton->setGeometry(Rect(buttonsBase, _backButton->preferredSize()));
  buttonsBase.x += _backButton->preferredSize().w + margin;
  _nextButton->setGeometry(Rect(buttonsBase, _nextButton->preferredSize()));

  Point previewLabelPos;
  previewLabelPos.x = clientGeometry().x + columnsWidth[0] + columnsWidth[1] + 2.0f * margin;
  previewLabelPos.y = clientGeometry().y + margin;

  _previewLabel->setGeometry(Rect(previewLabelPos, _previewLabel->preferredSize()));

  _textureFrame.x = _previewLabel->geometry().x;
  _textureFrame.y = _previewLabel->geometry().y2() + margin;
  _textureFrame.w = clientGeometry().w - columnsWidth[0] - columnsWidth[1] - 3.0f * margin;
  _textureFrame.h = _textureFrame.w;
}

void MapDialog::keyboardDownEvent(KeyboardDownEvent* e)
{
  Dialog::keyboardDownEvent(e);

  if (e->event().keysym.sym == SDLK_ESCAPE)
  {
    sendChildEvent(Cancelled);
  }
}

void MapDialog::childEvent(Widget *sender, int parameter)
{
  if (sender == _distributionChoiceBox)
  {
    FractalOptions o = _fractal.options();
    string s1, s2;
    switch (_distributionChoiceBox->index())
    {
      case 0:
      {
        o.distribution = DT_Uniform;
        _param1Label->setText("Min.:");
        _param2Label->setText("Max.:");
        s1 = toString<float>(o.distributionUniformMin);
        s2 = toString<float>(o.distributionUniformMax);
        break;
      }
      case 1:
      {
        o.distribution = DT_Normal;
        _param1Label->setText("Mean:");
        _param2Label->setText("Std deviation:");
        s1 = toString<float>(o.distributionNormalMean);
        s2 = toString<float>(o.distributionNormalVariance);
        break;
      }
      case 2:
      {
        o.distribution = DT_Weibull;
        _param1Label->setText("Scale:");
        _param2Label->setText("Shape:");
        s1 = toString<float>(o.distributionWeibullScale);
        s2 = toString<float>(o.distributionWeibullShape);
        break;
      }
      default: {}
    }
    _param1Edit->setText(s1);
    _param2Edit->setText(s2);
    resizeEvent();

    _fractal.setOptions(o);
    generateTexture();
  }
  else if (sender == _clampingChoiceBox)
  {
    FractalOptions o = _fractal.options();
    stringstream s1, s2;
    switch (_clampingChoiceBox->index())
    {
      case 0:
      {
        o.clamping = CM_Round;
        break;
      }
      case 1:
      {
        o.clamping = CM_Loop;
        break;
      }
      default: {}
    }
    _fractal.setOptions(o);
    generateTexture();
  }
  else if (sender == _mixingChoiceBox)
  {
    FractalOptions o = _fractal.options();
    string s1, s2;
    switch (_mixingChoiceBox->index())
    {
      case 0:
      {
        o.mixing = MM_Gauss;
        s1 = toString(o.mixingTwopointGaussCutoff);
        s2 = toString(o.mixingFourpointGaussCutoff);
        break;
      }
      case 1:
      {
        o.mixing = MM_Linear;
        s1 = toString(o.mixingTwopointLinearMin);
        s2 = toString(o.mixingFourpointLinearMin);
        break;
      }
      default: {}
    }
    _mixingTwopointEdit->setText(s1);
    _mixingFourpointEdit->setText(s2);
    resizeEvent();

    _fractal.setOptions(o);
    generateTexture();
  }
  else if (sender == _backButton)
  {
    sendChildEvent(Cancelled);
  }
  else if (sender == _nextButton)
  {
    sendChildEvent(Okeyed);
  }
  else if ((parameter == LineEdit::ReturnPressed) ||
           (parameter == LineEdit::TextChangedOnFocusOut))
  {
    readControls();
  }
}

void MapDialog::readControls()
{
  _scale.x = fromString<float>(_xEdit->text());
  _scale.y = fromString<float>(_yEdit->text());
  _scale.z = fromString<float>(_zEdit->text());

  _scale.clamp(Vector3D(0.1f, 0.1f, 0.1f), Vector3D(10000.0f, 10000.0f, 10000.0f));

  FractalOptions options = _fractal.options();

  switch (options.distribution)
  {
    case DT_Uniform:
    {
      options.distributionUniformMin = fromString<float>(_param1Edit->text());
      options.distributionUniformMax = fromString<float>(_param2Edit->text());
      break;
    }
    case DT_Normal:
    {
      options.distributionNormalMean = fromString<float>(_param1Edit->text());
      options.distributionNormalVariance = fromString<float>(_param2Edit->text());
      break;
    }
    case DT_Weibull:
    {
      options.distributionWeibullScale = fromString<float>(_param1Edit->text());
      options.distributionWeibullShape = fromString<float>(_param2Edit->text());
      break;
    }
  }

  options.clampingMin = fromString<float>(_minEdit->text());
  options.clampingMax = fromString<float>(_maxEdit->text());

  switch (options.mixing)
  {
    case MM_Gauss:
    {
      options.mixingTwopointGaussCutoff = fromString<float>(_mixingTwopointEdit->text());
      options.mixingFourpointGaussCutoff = fromString<float>(_mixingFourpointEdit->text());
      break;
    }
    case MM_Linear:
    {
      options.mixingTwopointLinearMin = fromString<float>(_mixingTwopointEdit->text());
      options.mixingFourpointLinearMin = fromString<float>(_mixingFourpointEdit->text());
      break;
    }
  }

  _fractal.setOptions(options);

  writeControls();
  generateTexture();
}

void MapDialog::writeControls()
{
  _xEdit->setText(toString<float>(_scale.x));
  _yEdit->setText(toString<float>(_scale.y));
  _zEdit->setText(toString<float>(_scale.z));

  FractalOptions options = _fractal.options();

  switch (options.distribution)
  {
    case DT_Uniform:
    {
      _param1Edit->setText(toString<float>(options.distributionUniformMin));
      _param2Edit->setText(toString<float>(options.distributionUniformMax));
      break;
    }
    case DT_Normal:
    {
      _param1Edit->setText(toString<float>(options.distributionNormalMean));
      _param2Edit->setText(toString<float>(options.distributionNormalVariance));
      break;
    }
    case DT_Weibull:
    {
      _param1Edit->setText(toString<float>(options.distributionWeibullScale));
      _param2Edit->setText(toString<float>(options.distributionWeibullShape));
      break;
    }
  }

  _minEdit->setText(toString<float>(options.clampingMin));
  _maxEdit->setText(toString<float>(options.clampingMax));

  switch (options.mixing)
  {
    case MM_Gauss:
    {
      _mixingTwopointEdit->setText(toString<float>(options.mixingTwopointGaussCutoff));
      _mixingFourpointEdit->setText(toString<float>(options.mixingFourpointGaussCutoff));
      break;
    }
    case MM_Linear:
    {
      _mixingTwopointEdit->setText(toString<float>(options.mixingTwopointLinearMin));
      _mixingFourpointEdit->setText(toString<float>(options.mixingFourpointLinearMin));
      break;
    }
  }
}

void MapDialog::generateTexture()
{
  if (_hasTexture)
  {
    glDeleteTextures(1, &_texture);
  }

  _fractal.clear();
  _fractal.generate(0);

  int s = _fractal.size() - 1;

  GLubyte *pixels = new GLubyte[3 * s * s];
  for (int y = 0; y < s; ++y)
  {
    for (int x = 0; x < s; ++x)
    {
      GLubyte v = (GLubyte)(_fractal.value(x, y) * 255);
      pixels[3 * (y * s + x) + 0] = v;
      pixels[3 * (y * s + x) + 1] = v;
      pixels[3 * (y * s + x) + 2] = v;
    }
  }

  glEnable(GL_TEXTURE_2D);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(1, &_texture);

  glBindTexture(GL_TEXTURE_2D, _texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, s, s, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  glDisable(GL_TEXTURE_2D);

  delete[] pixels;
  _hasTexture = true;
}
