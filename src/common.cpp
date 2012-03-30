/***************************************************************************
 *   Copyright (C) 2011-2012 by Piotr Dziwinski                            *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* common.cpp
    Contains the implementation of structs and classes from common.h. */

#include "common.h"

#include <cmath>

using namespace std;

const float TOLERANCE = 0.001f;
const float PI_180 = M_PI / 180.0f;
const float _180_PI = 180.0f / M_PI;


bool Rect::pointInside(const Point &point) const
{
  if ((point.x >= x) && (point.y >= y) &&
      (point.x <= x + w) && (point.y <= y + h))
    return true;

  return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void Color::fromChar(char pR, char pG, char pB, char pA)
{
  r = pR / 255.0f;
  g = pG / 255.0f;
  b = pB / 255.0f;
  a = pA / 255.0f;
}

void Color::fromFloat(float pR, float pG, float pB, float pA)
{
  r = pR;
  g = pG;
  b = pB;
  a = pA;
}

void Color::fromSDL_Color(const SDL_Color &color)
{
  r = color.r / 255.0f;
  g = color.g / 255.0f;
  b = color.b / 255.0f;
  a = 1.0f;
}

void Color::toChar(char &pR, char &pG, char &pB, char &pA) const
{
  pR = (char)(r * 255.0f);
  pG = (char)(g * 255.0f);
  pB = (char)(b * 255.0f);
  pA = (char)(a * 255.0f);
}

void Color::toFloat(float &pR, float &pG, float &pB, float &pA) const
{
  pR = r;
  pG = g;
  pB = b;
  pA = a;
}

SDL_Color Color::toSDL_Color() const
{
  SDL_Color color;
  color.r = (char)(r * 255.0f);
  color.g = (char)(g * 255.0f);
  color.b = (char)(b * 255.0f);
  return color;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

float Size::diagonal() const
{
  return sqrt(w * w + h * h);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

float Vector3D::length() const
{
  return sqrt(x * x + y * y + z * z);
}

Vector3D Vector3D::operator-() const
{
  Vector3D result;
  result.x = -x;
  result.y = -y;
  result.z = -z;
  return result;
}

const Vector3D& Vector3D::operator+=(const Vector3D &vector)
{
  x += vector.x;
  y += vector.y;
  z += vector.z;
  return *this;
}

const Vector3D& Vector3D::operator-=(const Vector3D &vector)
{
  x -= vector.x;
  y -= vector.y;
  z -= vector.z;
  return *this;
}

const Vector3D& Vector3D::operator*=(float value)
{
  x *= value;
  y *= value;
  z *= value;
  return *this;
}

const Vector3D& Vector3D::operator/=(float value)
{
  x /= value;
  y /= value;
  z /= value;
  return *this;
}

Vector3D operator+(const Vector3D &left, const Vector3D &right)
{
  Vector3D result;
  result.x = left.x + right.x;
  result.y = left.y + right.y;
  result.z = left.z + right.z;
  return result;
}

Vector3D operator-(const Vector3D &left, const Vector3D &right)
{
  Vector3D result;
  result.x = left.x - right.x;
  result.y = left.y - right.y;
  result.z = left.z - right.z;
  return result;
}

Vector3D operator*(float left, const Vector3D &right)
{
  return operator*(right, left);
}

Vector3D operator*(const Vector3D &left, float right)
{
  Vector3D result;
  result.x = left.x * right;
  result.y = left.y * right;
  result.z = left.z * right;
  return result;
}

Vector3D operator/(const Vector3D &left, float right)
{
  Vector3D result;
  result.x = left.x / right;
  result.y = left.y / right;
  result.z = left.z / right;
  return result;
}

float Vector3D::cosAngle(const Vector3D &right) const
{
  return dotProduct(right) / (length() * right.length());
}

float Vector3D::angle(const Vector3D &right) const
{
  return _180_PI * acos(dotProduct(right) / (length() * right.length()));
}

float Vector3D::dotProduct(const Vector3D &right) const
{
  return x * right.x + y * right.y + z * right.z;
}

Vector3D Vector3D::crossProduct(const Vector3D &right) const
{
  Vector3D result;
  result.x = y * right.z - z * right.y;
  result.y = z * right.x - x * right.z;
  result.z = x * right.y - y * right.x;
  return result;
}

void Vector3D::normalize()
{
  float sqLen = x*x + y*y + z*z;
  if ((fabs(sqLen) > TOLERANCE) && (fabs(1.0f - sqLen) > TOLERANCE))
  {
    float len = sqrt(sqLen);
    x /= len;
    y /= len;
    z /= len;
  }
}

void Vector3D::rotate(float angle, const Vector3D &axis)
{
  float c = cos(angle * PI_180);
  float s = sin(angle * PI_180);

  float ax = axis.x;
  float ay = axis.y;
  float az = axis.z;

  float m[3][3] =
  {
    { ax*ax * (1-c) + c   , ay*ax * (1-c) - az*s, az*ax * (1-c) + ay*s },
    { ax*ay * (1-c) + az*s, ay*ay * (1-c) + c   , az*ay * (1-c) - ax*s },
    { ax*az * (1-c) - ay*s, ay*az * (1-c) + ax*s, az*az * (1-c) + c    }
  };

  float nx = x * m[0][0] + y * m[0][1] + z * m[0][2];
  float ny = x * m[1][0] + y * m[1][1] + z * m[1][2];
  float nz = x * m[2][0] + y * m[2][1] + z * m[2][2];
  x = nx;
  y = ny;
  z = nz;
}

void Vector3D::clamp(const Vector3D &minValues, const Vector3D &maxValues)
{
  if (x < minValues.x)
    x = minValues.x;
  else if (x > maxValues.x)
    x = maxValues.x;

  if (y < minValues.y)
    y = minValues.y;
  else if (y > maxValues.y)
    y = maxValues.y;

  if (z < minValues.z)
    z = minValues.z;
  else if (z > maxValues.z)
    z = maxValues.z;
}

bool Vector3D::between(const Vector3D &minValues, const Vector3D &maxValues)
{
  if (x <= minValues.x)
    return false;
  if (y <= minValues.y)
    return false;
  if (z <= minValues.z)
    return false;

  if (x >= maxValues.x)
    return false;
  if (y >= maxValues.y)
    return false;
  if (z >= maxValues.z)
    return false;

  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#if defined(__linux__)

#include <sys/time.h>
#include <time.h>

struct TimePrivate
{
  timespec clockTime;

  TimePrivate()
    { clockTime.tv_sec = clockTime.tv_nsec = 0; }
};

#elif defined(WIN32) || defined(_WIN32)

#include <windows.h>

struct TimePrivate
{
  FILETIME fileTime;

  TimePrivate()
    { fileTime.dwHighDateTime = fileTime.dwLowDateTime = 0; }
};

#else

struct TimePrivate
{
  Uint32 sdlTicks;

  TimePrivate()
    { sdlTicks = 0; }
};

#endif

Time::~Time()
{
  delete _private;
  _private = NULL;
}

Time::Time(const Time& t)
{
  _private = new TimePrivate();
  *_private = *t._private;
}

const Time& Time::operator=(const Time& t)
{
  *_private = *t._private;
  return *this;
}

Time* Time::currentTime()
{
  Time *result = new Time();
  result->_private = new TimePrivate();

  #if defined(__linux__)

  clock_gettime(CLOCK_MONOTONIC, &result->_private->clockTime);

  #elif defined(WIN32) || defined(_WIN32)

  GetSystemTimeAsFileTime(&result->_private->fileTime);

  #else

  result->_private.sdlTicks = SDL_GetTicks();

  #endif

  return result;
}

int Time::resolution()
{
  #if defined(__linux__)

  return 1;

  #elif defined(WIN32) || defined(_WIN32)

  return 100;

  #else

  return 1000000;

  #endif
}

long long Time::difference(Time *other)
{
  #if defined(__linux__)

  return  this->_private->clockTime.tv_nsec - other->_private->clockTime.tv_nsec +
          1000000000 * (this->_private->clockTime.tv_sec - other->_private->clockTime.tv_sec);

  #elif defined(WIN32) || defined(_WIN32)

  long long tH = (1ll << 32) * (this->_private->fileTime.dwHighDateTime -
                 other->_private->fileTime.dwHighDateTime);
  long long tL = this->_private->fileTime.dwLowDateTime - 
				 other->_private->fileTime.dwLowDateTime;
  return 100 * (tH + tL);

  #else

  return 1000000 * (this->_private->sdlTicks - other->_private->sdlTicks);

  #endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Timer::Timer(unsigned long long pInterval)
{
  _intervalBeginTime = _intervalEndTime = NULL;
  _interval = pInterval;
  _timeoutCount = 0;
  _enabled = true;
  reset();
}

Timer::~Timer()
{
  delete _intervalBeginTime;
  delete _intervalEndTime;
  _intervalBeginTime = _intervalEndTime = NULL;
}

Timer::Timer(const Timer &t)
{
  _interval = t._interval;
  _timeoutCount = t._timeoutCount;
  _enabled = t._enabled;

  _intervalBeginTime = new Time(*t._intervalBeginTime);
  _intervalEndTime = new Time(*t._intervalEndTime);
}

const Timer& Timer::operator=(const Timer &t)
{
  _interval = t._interval;
  _timeoutCount = t._timeoutCount;
  _enabled = t._enabled;

  *_intervalBeginTime = *t._intervalBeginTime;
  *_intervalEndTime = *t._intervalEndTime;
  return *this;
}

void Timer::reset()
{
  if (_intervalBeginTime != NULL)
    delete _intervalBeginTime;
  if (_intervalEndTime != NULL)
    delete _intervalEndTime;

  _intervalBeginTime = Time::currentTime();
  _intervalEndTime = Time::currentTime();

  _timeoutCount = 0;
}

unsigned long long Timer::timeoutDifference()
{
  return (unsigned long long)(_intervalEndTime->difference(_intervalBeginTime));
}

bool Timer::checkTimeout()
{
  if (!_enabled)
    return false;

  Time *currentTime = Time::currentTime();

  if (((unsigned long long)currentTime->difference(_intervalEndTime)) >= _interval)
  {
    delete _intervalBeginTime;
    _intervalBeginTime = _intervalEndTime;
    _intervalEndTime = currentTime;
    ++_timeoutCount;
    return true;
  }

  delete currentTime;
  return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

string unicodeCharToUtf8(Uint16 ch)
{
  string result;
  if (ch < 0x0080)
  {
    result += (char)(ch);
  }
  else if (ch < 0x0800)
  {
    char ch1 = 0xC0 | ((ch & 0x07C0) >> 6);
    char ch2 = 0x80 | (ch & 0x3F);
    result += ch1;
    result += ch2;
  }
  else
  {
    char ch1 = 0xE0 | ((ch & 0xF000) >> 12);
    char ch2 = 0x80 | ((ch & 0x07C0) >> 6);
    char ch3 = 0x80 | (ch & 0x3F);
    result += ch1;
    result += ch2;
    result += ch3;
  }
  return result;
}

string unicodeStringToUtf8(const wstring &str)
{
  string result;
  for (unsigned int i = 0; i < str.size(); ++i)
    result += unicodeCharToUtf8((Uint16)str[i]);

  return result;
}

Uint16 utf8CharToUnicode(const string &ch)
{
  if (ch.empty())
    return 0;

  Uint16 result = 0;
  if ((ch[0] & 0x80) == 0)
  {
    if (ch.size() == 1)
      result = (Uint16)ch[0];
  }
  else if ((ch[0] & 0xC0) == 0xC0)
  {
    if (ch.size() == 2)
    {
      Uint16 ch1 = (ch[0] & 0x1F) << 6;
      Uint16 ch2 = (ch[1] & 0x3F);
      result = ch1 | ch2;
    }
  }
  else
  {
    if (ch.size() == 3)
    {
      Uint16 ch1 = (ch[0] & 0xF0) << 12;
      Uint16 ch2 = (ch[1] & 0xC0) << 6;
      Uint16 ch3 = (ch[2] & 0xC0);
      result = ch1 | ch2 | ch3;
    }
  }

  return result;
}

wstring utf8StringToUnicode(const string &str)
{
  wstring result;
  unsigned int pos = 0;
  while (pos < str.size())
  {
    int len = nextUtf8CharLength(str, pos);
    if (len == 0)
      break;

    string ch = str.substr(pos, len);
    result += (wchar_t)(utf8CharToUnicode(ch));
    pos += len;
  }
  return result;
}

int nextUtf8CharLength(const string &str, unsigned int pos)
{
  if (pos >= str.size())
    return 0;

  if ((str[pos] & 0x80) == 0)
    return 1;
  else if ((str[pos] & 0xC0) == 0xC0)
    return 2;
  else
    return 3;

  return 0;
}

size_t utf8StringLength(const string &str)
{
  size_t result = 0;
  for (unsigned int i = 0; i < str.size(); ++i)
  {
    char ch = str[i];
    if ((ch & 0x80) == 0)
      ++result;
    else if ((ch & 0xC0) == 0xC0)
      result += 2;
    else
      result += 3;
  }
  return result;
}

std::string replace(const std::string &str, const std::string &oldStr, const std::string &newStr)
{
  string result = str;
  size_t pos = 0;
  while((pos = str.find(oldStr, pos)) != std::string::npos)
  {
    result.replace(pos, oldStr.length(), newStr);
    pos += newStr.length();
  }
  return result;
}
