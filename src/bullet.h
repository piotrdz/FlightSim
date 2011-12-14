/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

/* bullet.h
   Zawiera klasę Bullet - pocisku wystrzeliwanego przez gracza. */

#pragma once

#include "common.h"

//! Pocisk wystrzeliwany przez samolot gracza
class Bullet
{
  public:
    Bullet(const Vector3D &pPosition, const Vector3D &pVelocity,
           const Vector3D &pSide, const Vector3D &pUp);

    //! Ustawia rozpad
    inline void setDecayed()
      { _decayed = true; }
    //! @{ Akcesory
    inline bool decayed() const
      { return _decayed; }
    const Vector3D position() const
      { return _position; }
    //! @}

    //! Wyświetla pocisk
    void render();

    //! Aktualizuje pozycję (deltaT to kwant czasu)
    void update(float deltaT);

  private:
    //! Czy pocisk "rozpadł się"
    bool _decayed;
    //! @{ Pozycja, prędkość, wektor "boku" i "góry" pocisku
    Vector3D _position, _velocity, _side, _up;
    //! @}
    Vector3D _startPosition;
    //! Odległość, po przebyciu której pocisk rozpada się
    static const float BULLET_DECAY_DISTANCE;
};
