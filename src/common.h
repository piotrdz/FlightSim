/***************************************************************************
 *   Copyright (C) 2011-2012 by Piotr Dziwinski                            *
 *   piotrdz@gmail.com                                                     *
 ***************************************************************************/

 /* common.h
    Contains shared structs and definitions for the whole project. */

#pragma once

#include "config.h"

#include <string>
#include <sstream>

#include <SDL/SDL.h>

#include <libintl.h>

// Gettext shortcut
#define _(s) gettext(s)


enum Alignment
{
  AL_Left = 0x1,
  AL_Right = 0x2,
  AL_HCenter = AL_Left | AL_Right,
  AL_Top = 0x4,
  AL_Bottom = 0x8,
  AL_VCenter = AL_Top | AL_Bottom,
  AL_Center = AL_HCenter | AL_VCenter
};

struct Color
{
  float r, g, b, a;

  explicit Color(float pR = 0.0f, float pG = 0.0f, float pB = 0.0f, float pA = 1.0f)
    : r(pR), g(pG), b(pB), a(pA) {}

  void fromChar(char pR, char pG, char pB, char pA);
  void fromFloat(float pR, float pG, float pB, float pA);
  void fromSDL_Color(const SDL_Color &color);

  void toChar(char &pR, char &pG, char &pB, char &pA) const;
  void toFloat(float &pR, float &pG, float &pB, float &pA) const;
  SDL_Color toSDL_Color() const;

  inline operator float*()
    { return (float*)(this); }

  inline operator const float*() const
    { return (const float*)(this); }
};

struct Point
{
  float x, y;

  explicit Point(float pX = 0.0f, float pY = 0.0f)
    : x(pX), y(pY) {}

  inline bool operator==(const Point &p) const
    { return x == p.x && y == p.y; }

  inline bool operator!=(const Point &p) const
    { return !operator==(p); }
};

struct Size
{
  float w, h;

  explicit Size(float pWidth = 0.0f, float pHeight = 0.0f)
    : w(pWidth), h(pHeight) {}

  float diagonal() const;

  inline bool operator==(const Size &s) const
    { return w == s.w && h == s.h; }

  inline bool operator!=(const Size &s) const
    { return !operator==(s); }
};

struct Rect : public Point, public Size
{
  explicit Rect(float pX = 0.0f, float pY = 0.0f, float pWidth = 0.0f, float pHeight = 0.0f)
    : Point(pX, pY), Size(pWidth, pHeight) {}
  Rect(const Point &pPosition, const Size &pSize)
    : Point(pPosition), Size(pSize) {}

  inline float x1() const
    { return x; }
  inline float y1() const
    { return y; }

  inline float xMid() const
    { return x + 0.5f * w; }
  inline float yMid() const
    { return y + 0.5f * h; }

  inline float x2() const
    { return x + w; }
  inline float y2() const
    { return y + h; }

  inline Point position() const
    { return *this; }
  inline Size size() const
    { return *this; }

  bool pointInside(const Point &point) const;

  inline bool operator==(const Rect &r) const
    { return Point::operator==(r) && Size::operator==(r); }

  inline bool operator!=(const Rect &r) const
    { return !operator==(r); }
};

// Constants used in calculations:
extern const float TOLERANCE;
extern const float PI_180;
extern const float _180_PI;


struct Vector3D
{
  float x, y, z;

  explicit Vector3D(float pX = 0.0f, float pY = 0.0f, float pZ = 0.0f)
    : x(pX), y(pY), z(pZ) {}

  // Operators for vectors and scalars

  Vector3D operator-() const;

  const Vector3D& operator+=(const Vector3D &vector);
  const Vector3D& operator-=(const Vector3D &vector);
  const Vector3D& operator*=(float value);
  const Vector3D& operator/=(float value);

  friend Vector3D operator+(const Vector3D &left, const Vector3D &right);
  friend Vector3D operator-(const Vector3D &left, const Vector3D &right);
  friend Vector3D operator*(float left, const Vector3D &right);
  friend Vector3D operator*(const Vector3D &left, float right);
  friend Vector3D operator/(const Vector3D &left, float right);

  // For casting to pointers:

  inline operator float*()
    { return (float*)(this); }

  inline operator const float*() const
    { return (const float*)(this); }

  float length() const;

  float cosAngle(const Vector3D &right) const;

  float angle(const Vector3D &right) const;


  float dotProduct(const Vector3D &right) const;

  inline static float dotProduct(const Vector3D &left, const Vector3D &right)
    { return left.dotProduct(right); }

  Vector3D crossProduct(const Vector3D &right) const;

  inline static Vector3D crossProduct(const Vector3D &left, const Vector3D &right)
    { return left.crossProduct(right); }


  void normalize();

  inline static Vector3D normalize(const Vector3D &vec)
  {
    Vector3D result = vec;
    result.normalize();
    return result;
  }

  void rotate(float angle, const Vector3D &axis);

  inline static Vector3D rotate(const Vector3D &vec, float angle, const Vector3D &axis)
  {
    Vector3D result = vec;
    result.rotate(angle, axis);
    return result;
  }

  void clamp(const Vector3D &minValues, const Vector3D &maxValues);

  inline static Vector3D clamp(const Vector3D &vec, const Vector3D &minValues,
                               const Vector3D &maxValues)
  {
    Vector3D result = vec;
    result.clamp(minValues, maxValues);
    return result;
  }

  bool between(const Vector3D &minValues, const Vector3D &maxValues);
};

// Private data for class Time
struct TimePrivate;

// Stores a timestamp; uses different clocks on different systems
class Time
{
  private:
    Time() : _private(NULL) {}

  public:
    ~Time();

    Time(const Time &t);
    const Time& operator=(const Time &t);

    // Result in nanoseconds (ns)
    static int resolution();

    static Time* currentTime();

    // Result in nanoseconds
    long long difference(Time *other);

  private:
    TimePrivate *_private;
};

// Tells whether a given time interval has passed
class Timer
{
  public:
    explicit Timer(unsigned long long pInterval = 10000000);
    ~Timer();

    Timer(const Timer &t);
    const Timer& operator=(const Timer &t);

    void reset();

    bool checkTimeout();

    inline unsigned long long interval() const
      { return _interval; }

    inline void setInterval(unsigned long long pInterval)
      { _interval = pInterval; }

    inline void setIntervalMsec(unsigned long long pInterval)
      { _interval = pInterval * 1000000; }

    unsigned long long timeoutDifference();

    inline unsigned int timeoutDifferenceMsec()
      { return timeoutDifference() * 1000000; }

    inline unsigned int timeoutCount() const
      { return _timeoutCount; }

    inline bool enabled() const
      { return _enabled; }

    inline void setEnabled(bool pEnabled)
      { _enabled = pEnabled; }

  private:
    unsigned long long _interval;
    Time *_intervalBeginTime, *_intervalEndTime;
    unsigned int _timeoutCount;
    bool _enabled;
};

template<class T>
std::string toString(T value, bool *ok = NULL)
{
  std::ostringstream s;
  s << value;
  if (ok != NULL)
    *ok = !s.fail();
  return s.str();
}

template<class T>
T fromString(const std::string &str, bool *ok = NULL)
{
  std::istringstream s;
  s.str(str);
  T value;
  s >> value;
  if (ok != NULL)
    *ok = !s.fail();
  return value;
}

//namespace StringUtils
//{
  std::string unicodeCharToUtf8(Uint16 ch);

  std::string unicodeStringToUtf8(const std::wstring &str);

  Uint16 utf8CharToUnicode(const std::string &ch);

  std::wstring utf8StringToUnicode(const std::string &str);

  int nextUtf8CharLength(const std::string &str, unsigned int pos);

  size_t utf8StringLength(const std::string &str);

  std::string replace(const std::string &str, const std::string &oldStr, const std::string &newStr);
//};
