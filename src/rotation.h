/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* rotation.h
    Zawiera strukturę kwaternionu Quaternion oraz klasę Rotation do opisywania
    obrotów samolotu w 3D. */

#pragma once

#include "config.h"

#include "common.h"

//! Kwaternion
struct Quaternion
{
  float w;
  float x, y, z;

  explicit Quaternion(float pW = 1.0f, float pX = 0.0f,
                      float pY = 0.0f, float pZ = 0.0f)
    { w = pW; x = pX; y = pY; z = pZ; }

  //! Zwraca normę
  float norm() const;
  //! Zwraca sprzężenie
  Quaternion conjugate() const;

  //! Normalizuje kwaternion
  void normalize();

  //! @{ Operatory
  Quaternion operator-() const;
  Quaternion operator+(const Quaternion &q) const;
  Quaternion operator-(const Quaternion &q) const;
  Quaternion operator*(const Quaternion &q) const;
  Quaternion operator*(float s) const;
  Quaternion operator/(float s) const;

  Vector3D operator*(const Vector3D &v) const;
  //! @}

  //! @{ Funkcje do przekształceń z innych reprezentacji
  void fromAxisAngle(const Vector3D &axis, float angle);
  void fromEulerAngles(float pitch, float yaw, float roll);
  void fromMatrix(const float (&m)[3][3]);
  //! @}

  //! @{ Funkcje do przekształceń do innych reprezentacji
  void toAxisAngle(Vector3D &axis, float &angle) const;
  void toEulerAngles(float &pitch, float &yaw, float &roll) const;
  void toMatrix(float (&m)[3][3]) const;
  //! @}
};

//! Klasa opisująca obrót
class Rotation
{
  public:
    Rotation();

    //! @{ Akcesory
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
    //! @}

    //! Ustawia obrót na zadane kąty
    void setAngles(float pitch, float heading, float roll);

    //! Obraca o podane kąty na osiach lokalnych
    void rotateLocal(float deltaX, float deltaY, float deltaZ);

    //! Obraca o podane kąty na osiach globalnych
    void rotateGlobal(float deltaX, float deltaY, float deltaZ);

    //! Wypełnia macierz transformacji w formacie OpenGL
    void toGLMatrix(float (&matrix)[16]) const;

    //! Wypełia macierz transformacji odwrotnej w formacie OpenGL
    void reverseToGLMatrix(float (&matrix)[16]) const;

  private:
    Quaternion _quaternion;
    Vector3D _mainAxis, _upAxis, _sideAxis;
    Vector3D _angles;

    //! Przelicza ponownie kąty i osie z kwaternionu
    void update();
};
