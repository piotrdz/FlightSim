/***************************************************************************
 *   Copyright (C) 2011-2012 by Piotr Dziwinski                            *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* bullet.cpp
    Contains implementation of class Bullet. */

#include "bullet.h"

#include <GL/gl.h>

const float Bullet::BULLET_DECAY_DISTANCE = 10000.0f;

Bullet::Bullet(const Vector3D& pPosition, const Vector3D& pVelocity,
               const Vector3D& pSide, const Vector3D& pUp)
{
  _startPosition = _position = pPosition;
  _velocity = pVelocity;
  _side = pSide;
  _up = pUp;
  _decayed = false;
}

void Bullet::render()
{
  if (_decayed)
    return;

  glColor3f(0.87f, 0.66f, 0.18f);

  Vector3D v1 = _position;
  Vector3D v2 = v1 + 20.0f * Vector3D::normalize(_velocity);
  Vector3D v3 = v1 - _side;
  Vector3D v4 = v1 + _up;
  v1 += _side;

  glBegin(GL_TRIANGLES);
  {
    glVertex3fv(v1);
    glVertex3fv(v3);
    glVertex3fv(v2);

    glVertex3fv(v3);
    glVertex3fv(v2);
    glVertex3fv(v4);

    glVertex3fv(v4);
    glVertex3fv(v2);
    glVertex3fv(v1);

    glVertex3fv(v1);
    glVertex3fv(v3);
    glVertex3fv(v4);
  }
  glEnd();
}

void Bullet::update(float deltaT)
{
  if (_decayed)
    return;

  _position += _velocity * deltaT;

  if ((_position - _startPosition).length() > BULLET_DECAY_DISTANCE)
    _decayed = true;
}
