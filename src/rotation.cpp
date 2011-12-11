/***************************************************************************
 *   Copyright (C) 2011 by Piotr Dziwinski                                 *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* rotation.cpp
    Zawiera implementację struktury Quaternion i klasy Rotation. */

#include "rotation.h"
#include "common.h"

#include <cmath>

using namespace std;


float Quaternion::norm() const
{
  return sqrt(w*w + x*x + y*y + z*z);
}

Quaternion Quaternion::conjugate() const
{
  return Quaternion(w, -x, -y, -z);
}

void Quaternion::normalize()
{
  float sqN = w*w + x*x + y*y + z*z;
  if ((fabs(sqN) > TOLERANCE) && (fabs(sqN - 1.0f) > TOLERANCE))
  {
    float n = sqrt(sqN);
    w /= n;
    x /= n;
    y /= n;
    z /= n;
  }
}

Quaternion Quaternion::operator-() const
{
  return Quaternion(-w, -x, -y, -z);
}

Quaternion Quaternion::operator+(const Quaternion &q) const
{
  return Quaternion(w + q.w, x + q.x, y + q.y, z + q.z);
}

Quaternion Quaternion::operator-(const Quaternion &q) const
{
  return Quaternion(w - q.w, x - q.x, y - q.y, z - q.z);
}

Quaternion Quaternion::operator*(const Quaternion &q) const
{
  return Quaternion(w * q.w - x * q.x - y * q.y - z * q.z,
                    w * q.x + x * q.w + y * q.z - z * q.y,
                    w * q.y - x * q.z + y * q.w + z * q.x,
                    w * q.z + x * q.y - y * q.x + z * q.w);
}

Quaternion Quaternion::operator*(float s) const
{
  return Quaternion(w * s, x * s, y * s, z * s);
}

Quaternion Quaternion::operator/(float s) const
{
  return Quaternion(w / s, x / s, y / s, z / s);
}

Vector3D Quaternion::operator*(const Vector3D &v) const
{
  Vector3D n = Vector3D::normalize(v);

  Quaternion vQ(0.0f, n.x, n.y, n.z);

  Quaternion result = vQ * conjugate();
  result = *this * result;

  return Vector3D(result.x, result.y, result.z);
}

void Quaternion::fromEulerAngles(float pitch, float yaw, float roll)
{
  float pV = 0.5f * pitch * PI_180;
  float yV = 0.5f * yaw   * PI_180;
  float rV = 0.5f * roll  * PI_180;

  float sinp = sin(pV);
  float siny = sin(yV);
  float sinr = sin(rV);

  float cosp = cos(pV);
  float cosy = cos(yV);
  float cosr = cos(rV);

  w = cosp * cosy * cosr + sinp * siny * sinr;
  x = sinp * cosy * cosr - cosp * siny * sinr;
  y = cosp * siny * cosr + sinp * cosy * sinr;
  z = cosp * cosy * sinr - sinp * siny * cosr;

  normalize();
}

void Quaternion::fromAxisAngle(const Vector3D &axis, float angle)
{
  angle *= 0.5f;

  Vector3D n = Vector3D::normalize(axis);
  float sinAngle = sin(angle * PI_180);

  w = cos(angle * PI_180);
  x = (n.x * sinAngle);
  y = (n.y * sinAngle);
  z = (n.z * sinAngle);
}

void Quaternion::fromMatrix(const float (&m)[3][3])
{
  float tr = m[0][0] + m[1][1] + m[2][2];

  if (tr > 0)
  {
    float S = 2.0f * sqrt(tr + 1.0f);
    w = 0.25f * S;
    x = (m[2][1] - m[1][2]) / S;
    y = (m[0][2] - m[2][0]) / S;
    z = (m[1][0] - m[0][1]) / S;
  }
  else if ((m[0][0] > m[1][1]) && (m[0][0] > m[2][2]))
  {
    float S = 2.0f * sqrt(1.0f + m[0][0] - m[1][1] - m[2][2]);
    w = (m[2][1] - m[1][2]) / S;
    x = 0.25f * S;
    y = (m[0][1] + m[1][0]) / S;
    z = (m[0][2] + m[2][0]) / S;
  }
  else if (m[1][1] > m[2][2])
  {
    float S = 2.0f * sqrt(1.0f + m[1][1] - m[0][0] - m[2][2]);
    w = (m[0][2] - m[2][0]) / S;
    x = (m[0][1] + m[1][0]) / S;
    y = 0.25 * S;
    z = (m[1][2] + m[2][1]) / S;
  }
  else
  {
    float S = 2.0f * sqrt(1.0f + m[2][2] - m[0][0] - m[1][1]);
    w = (m[1][0] - m[0][1]) / S;
    x = (m[0][2] + m[2][0]) / S;
    y = (m[1][2] + m[2][1]) / S;
    z = 0.25f * S;
  }
}

void Quaternion::toEulerAngles(float &pitch, float &yaw, float &roll) const
{
  pitch = _180_PI * atan2(2.0f * (w* x + y* z), 1 - 2.0f * (x* x + y* y));
  yaw   = _180_PI  * asin(2.0f * (w* y - x* z));
  roll  = _180_PI * atan2(2.0f * (w* z + x* y), 1 - 2.0f * (y* y + z* z));
}

void Quaternion::toAxisAngle(Vector3D &axis, float &angle) const
{
  float scale = sqrt(x * x + y * y + z * z);
  axis.x = x / scale;
  axis.y = y / scale;
  axis.z = z / scale;
  angle = 2.0f * _180_PI * acos(w);
}

void Quaternion::toMatrix(float (&m)[3][3]) const
{
  float x2 = x * x;
  float y2 = y * y;
  float z2 = z * z;
  float xy = x * y;
  float xz = x * z;
  float yz = y * z;
  float wx = w * x;
  float wy = w * y;
  float wz = w * z;

  m[0][0] = 1.0f - 2.0f * (y2 + z2);
  m[0][1] = 2.0f * (xy - wz);
  m[0][2] = 2.0f * (xz + wy);

  m[1][0] = 2.0f * (xy + wz);
  m[1][1] = 1.0f - 2.0f * (x2 + z2);
  m[1][2] = 2.0f * (yz - wx);

  m[2][0] = 2.0f * (xz - wy);
  m[2][1] = 2.0f * (yz + wx);
  m[2][2] = 1.0f - 2.0f * (x2 + y2);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Rotation::Rotation()
{
  _quaternion = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
  update();
}

void Rotation::setAngles(float pitch, float heading, float roll)
{
  _quaternion.fromAxisAngle(Vector3D(0.0f, 1.0f, 0.0f), -heading);
  rotateLocal(pitch, 0.0f, roll);
}

void Rotation::rotateLocal(float deltaX, float deltaY, float deltaZ)
{
  Quaternion qX, qY, qZ;
  qX.fromAxisAngle(Vector3D(1.0f, 0.0f, 0.0f), deltaX);
  qY.fromAxisAngle(Vector3D(0.0f, 1.0f, 0.0f), deltaY);
  qZ.fromAxisAngle(Vector3D(0.0f, 0.0f, 1.0f), deltaZ);

  _quaternion = _quaternion * qX;
  _quaternion = _quaternion * qY;
  _quaternion = _quaternion * qZ;
  _quaternion.normalize();

  update();
}

void Rotation::rotateGlobal(float deltaX, float deltaY, float deltaZ)
{
  Quaternion qX, qY, qZ;
  qX.fromAxisAngle(Vector3D(1.0f, 0.0f, 0.0f), deltaX);
  qY.fromAxisAngle(Vector3D(0.0f, 1.0f, 0.0f), deltaY);
  qZ.fromAxisAngle(Vector3D(0.0f, 0.0f, 1.0f), deltaZ);

  _quaternion = qX * _quaternion;
  _quaternion = qY * _quaternion;
  _quaternion = qZ * _quaternion;
  _quaternion.normalize();

  update();
}

void Rotation::toGLMatrix(float (&matrix)[16]) const
{
  float qm[3][3] = { {0.0f} };
  _quaternion.toMatrix(qm);

  matrix[0 ] = qm[0][0];
  matrix[1 ] = qm[0][1];
  matrix[2 ] = qm[0][2];
  matrix[3 ] = 0.0f;

  matrix[4 ] = qm[1][0];
  matrix[5 ] = qm[1][1];
  matrix[6 ] = qm[1][2];
  matrix[7 ] = 0.0f;

  matrix[8 ] = qm[2][0];
  matrix[9 ] = qm[2][1];
  matrix[10] = qm[2][2];
  matrix[11] = 0.0f;

  matrix[12] = 0.0f;
  matrix[13] = 0.0f;
  matrix[14] = 0.0f;
  matrix[15] = 1.0f;
}

void Rotation::reverseToGLMatrix(float (&matrix)[16]) const
{
  Rotation r;
  r._quaternion = _quaternion.conjugate();
  r.toGLMatrix(matrix);
}

void Rotation::update()
{
  // Wyznaczenie osi lokalnych z macierzy obrotu

  float qm[3][3] = { {0.0f} };
  _quaternion.toMatrix(qm);

  _mainAxis.x = qm[0][2];
  _mainAxis.y = qm[1][2];
  _mainAxis.z = qm[2][2];
  _mainAxis.normalize();

  _upAxis.x = qm[0][1];
  _upAxis.y = qm[1][1];
  _upAxis.z = qm[2][1];
  _upAxis.normalize();

  _sideAxis.x = qm[0][0];
  _sideAxis.y = qm[1][0];
  _sideAxis.z = qm[2][0];
  _sideAxis.normalize();

  // Wyliczenie kątu x - nachylenie do horyzontu (płaszczyzny XZ):
  _angles.x = _180_PI * asin(_mainAxis.y);

  /* Wyliczenie kątu y - kierunku lotu, czyli kąta między
        osią główną samolotu, a globalną osią Z: */
  Vector3D mainAxisXZ(_mainAxis.x, 0.0f, _mainAxis.z);
  _angles.y = atan2(_mainAxis.x, _mainAxis.z);

  if (isnan(_angles.y))
    _angles.y = 0.0f;

  _angles.y = _180_PI * _angles.y;

  if (_angles.y < 0.0f)
    _angles.y = 360.0 + _angles.y;

  if (_angles.y > 360.0f)
    _angles.y = 360.0f - _angles.y;

  // Wyliczenie kątu z - obrotu wzdłuż osi głównej samolotu:
  Vector3D mainUpDiff = Vector3D(0.0f, 1.0f, 0.0f) - _mainAxis;
  Vector3D unrotSideAxis = mainUpDiff.crossProduct(_mainAxis);
  _angles.z = _sideAxis.angle(unrotSideAxis);

  if (isnan(_angles.z))
    _angles.z = 0.0f;

  if (_sideAxis.y > 0.0f)
    _angles.z = -_angles.z;
}
