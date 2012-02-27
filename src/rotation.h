/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* rotation.h
    Contains the struct Quaternion and class Rotation to describe rotations
    of the plane in 3D. */

#pragma once

#include "config.h"

#include "common.h"

struct Quaternion
{
  float w;
  float x, y, z;

  explicit Quaternion(float pW = 1.0f, float pX = 0.0f,
                      float pY = 0.0f, float pZ = 0.0f)
    { w = pW; x = pX; y = pY; z = pZ; }

  float norm() const;
  Quaternion conjugate() const;

  void normalize();

  Quaternion operator-() const;
  Quaternion operator+(const Quaternion &q) const;
  Quaternion operator-(const Quaternion &q) const;
  Quaternion operator*(const Quaternion &q) const;
  Quaternion operator*(float s) const;
  Quaternion operator/(float s) const;

  Vector3D operator*(const Vector3D &v) const;

  void fromAxisAngle(const Vector3D &axis, float angle);
  void fromEulerAngles(float pitch, float yaw, float roll);
  void fromMatrix(const float (&m)[3][3]);

  void toAxisAngle(Vector3D &axis, float &angle) const;
  void toEulerAngles(float &pitch, float &yaw, float &roll) const;
  void toMatrix(float (&m)[3][3]) const;
};

class Rotation
{
  public:
    Rotation();

    inline const Quaternion& quaternion() const
      { return _quaternion; }

    inline const Vector3D& mainAxis() const
      { return _mainAxis; }

    inline const Vector3D& upAxis() const
      { return _upAxis; }

    inline const Vector3D& sideAxis() const
      { return _sideAxis; }

    inline float pitch() const
      { return _angles.x; }

    inline float heading() const
      { return _angles.y; }

    inline float roll() const
      { return _angles.z; }

    void setAngles(float pitch, float heading, float roll);

    void rotateLocal(float deltaX, float deltaY, float deltaZ);

    void rotateGlobal(float deltaX, float deltaY, float deltaZ);

    void toGLMatrix(float (&matrix)[16]) const;

    void reverseToGLMatrix(float (&matrix)[16]) const;

  private:
    Quaternion _quaternion;
    Vector3D _mainAxis, _upAxis, _sideAxis;
    Vector3D _angles;

    void update();
};
