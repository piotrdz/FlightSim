/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* model.h
    Contains the Model class, which is responsible for loading and
    rendering of models in PLY format. */

#pragma once

#include "config.h"

#include "common.h"
#include "object.h"

#include <GL/gl.h>

class Model : public Object
{
  public:
    Model(const std::string &pName = "");
    virtual ~Model();

    inline bool valid() const
      { return _valid; }

    bool load(const std::string &pFileName);

    inline Vector3D boundingBoxMin() const
      { return _boundMin; }

    inline Vector3D boundingBoxMax() const
      { return _boundMax; }

    inline float boundingBoxDiagonal() const
      { return (_boundMax - _boundMin).length(); }

    void render();

  private:
    bool _valid;
    unsigned int _list;
    Vector3D _boundMin, _boundMax;

    void destroy();
};
