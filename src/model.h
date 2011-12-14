/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* model.h
    Zawiera klasę Model, która odpowiada za wczytywanie i renderowanie
    modeli z plików w formacie PLY. */

#pragma once

#include "config.h"

#include "common.h"
#include "object.h"

#include <GL/gl.h>

//! Model w formacie PLY
class Model : public Object
{
  public:
    Model(const std::string &pName = "");
    virtual ~Model();

    //! Akcesor
    inline bool valid() const
      { return _valid; }

    //! Wczytuje model z pliku
    bool load(const std::string &pFileName);

    //! @{ Akcesory dla prostopadłościanu opisującego model
    inline Vector3D boundingBoxMin() const
      { return _boundMin; }

    inline Vector3D boundingBoxMax() const
      { return _boundMax; }

    inline float boundingBoxDiagonal() const
      { return (_boundMax - _boundMin).length(); }
    //! @}

    //! Wyświetla model
    void render();

  private:
    //! Czy model zawiera poprawne dane?
    bool _valid;
    //! ID listy
    unsigned int _list;
    //! @{ Prostopadłościan opisujący model
    Vector3D _boundMin, _boundMax;
    //! @}

    //! Niszczenie listy
    void destroy();
};
