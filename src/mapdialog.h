/***************************************************************************
 *   Copyright (C) 2011-2012 by Piotr Dziwinski                            *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* mapdialog.h
    Contains the MapDialog class - a dialog for setting map parameters. */

#pragma once

#include "config.h"

#include "dialog.h"
#include "controls.h"
#include "fractal.h"
#include "common.h"

class MapDialog : public Dialog
{
  public:
    enum Actions
    {
      Cancelled,
      Okeyed
    };

  public:
    MapDialog(Widget *pParent);
    virtual ~MapDialog();

    virtual void init();
    virtual void render();

    Vector3D scale() const
      { return _scale; }

    FractalOptions fractalOptions()
      { return _fractal.options(); }

  protected:
    virtual void resizeEvent();

    virtual void keyboardDownEvent(KeyboardDownEvent *e);

    virtual void childEvent(Widget *sender, int parameter);

  private:
    Fractal _fractal;
    Vector3D _scale;

    bool _hasTexture;
    unsigned int _texture;
    Rect _textureFrame;

    Label *_distributionLabel;
    ChoiceBox *_distributionChoiceBox;
    Label *_param1Label, *_param2Label;
    LineEdit *_param1Edit, *_param2Edit;

    Label *_clampingLabel;
    ChoiceBox *_clampingChoiceBox;
    Label *_minLabel, *_maxLabel;
    LineEdit *_minEdit, *_maxEdit;

    Label *_mixingLabel;
    ChoiceBox *_mixingChoiceBox;
    Label *_mixingTwopointLabel, *_mixingFourpointLabel;
    LineEdit *_mixingTwopointEdit, *_mixingFourpointEdit;

    Label *_scaleLabel;
    Label *_xLabel, *_yLabel, *_zLabel;
    LineEdit *_xEdit, *_yEdit, *_zEdit;

    Label *_previewLabel;

    Button *_nextButton, *_backButton;

    void readControls();
    void writeControls();
    void generateTexture();
};
