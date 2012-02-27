/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

/* bullet.h
   Contains the class Bullet - a bullet fired by a player. */

#pragma once

#include "common.h"

class Bullet
{
  public:
    Bullet(const Vector3D &pPosition, const Vector3D &pVelocity,
           const Vector3D &pSide, const Vector3D &pUp);

    inline void setDecayed()
      { _decayed = true; }
    inline bool decayed() const
      { return _decayed; }
    const Vector3D position() const
      { return _position; }

    void render();

    void update(float deltaT);

  private:
    bool _decayed;
    Vector3D _position, _velocity, _side, _up;
    Vector3D _startPosition;
    static const float BULLET_DECAY_DISTANCE;
};
